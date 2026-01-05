#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/mesh_asset.h"

#include "asset_loader/include/asset_handle.h"

#include "mono_forge_model/include/mfm.h"
#include "utility_header/file_loader.h"

#include "render_graph/include/geometry_pass.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service.h"

namespace mono_asset_extension
{

MeshAsset::MeshAsset(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
}

MeshAsset::~MeshAsset()
{
    // Create graphics service command list to delete buffers
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to cast to GraphicsCommandList");

    // Destroy vertex buffers
    for (const render_graph::ResourceHandle& vertex_buffer_handle : vertex_buffer_handles_)
        graphics_command_list->DestroyResource(&vertex_buffer_handle);

    // Destroy index buffer
    for (const render_graph::ResourceHandle& index_buffer_handle : index_buffer_handles_)
        graphics_command_list->DestroyResource(&index_buffer_handle);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

bool MeshAsset::Setup(
    const std::vector<const geometry::Geometry::Vertex*>& vertex_data, const std::vector<uint32_t>& vertex_count,
    const std::vector<const uint32_t*>& index_data, const std::vector<uint32_t>& index_count)
{
    // Resize handle vectors
    vertex_buffer_handles_.resize(vertex_data.size());
    index_buffer_handles_.resize(index_data.size());
    index_counts_.resize(index_data.size());

    // Store index counts
    for (size_t i = 0; i < index_count.size(); ++i)
        index_counts_[i] = index_count[i];

    // Create graphics service command list to create buffers
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to cast to GraphicsCommandList");

    // Check the vertex layout size same as geometry vertex size
    // Be sure to check the layout, as it is not possible to check if it is equal to the layout here.
    static_assert(
        sizeof(geometry::Geometry::Vertex) == sizeof(render_graph::geometry_pass::Vertex)
        && "Vertex size mismatch between geometry and render graph");
    
    // Check the index layout size same as geometry index size
    // Be sure to check the layout, as it is not possible to check if it is equal to the layout here.
    static_assert(
        sizeof(uint32_t) == sizeof(render_graph::geometry_pass::Index)
        && "Index size mismatch between geometry and render graph");

    // Create vertex buffers
    for (size_t i = 0; i < vertex_data.size(); ++i)
    {
        const render_graph::geometry_pass::Vertex* vertex_data_rg
            = reinterpret_cast<const render_graph::geometry_pass::Vertex*>(vertex_data[i]);
        graphics_command_list->CreateVertexBuffer(
            &vertex_buffer_handles_[i], vertex_count[i], vertex_data_rg);
    }

    // Create index buffer
    for (size_t i = 0; i < index_data.size(); ++i)
    {
        const uint32_t* index_data_rg = reinterpret_cast<const uint32_t*>(index_data[i]);
        graphics_command_list->CreateIndexBuffer(&index_buffer_handles_[i], index_count[i], index_data_rg);
    }

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));

    return true;
}

const std::vector<render_graph::ResourceHandle>* MeshAsset::GetVertexBufferHandles() const
{
    assert(IsSetup() && "MeshAsset is not set up");

    for (const auto& handle : vertex_buffer_handles_)
        assert(handle.IsValid() && "Vertex buffer handle is not valid");

    return &vertex_buffer_handles_;
}

const std::vector<render_graph::ResourceHandle>* MeshAsset::GetIndexBufferHandles() const
{
    assert(IsSetup() && "MeshAsset is not set up");

    for (const auto& handle : index_buffer_handles_)
        assert(handle.IsValid() && "Index buffer handle is not valid");

    return &index_buffer_handles_;
}

const std::vector<uint32_t>* MeshAsset::GetIndexCounts() const
{
    assert(IsSetup() && "MeshAsset is not set up");
    return &index_counts_;
}

MeshAssetSourceData::MeshAssetSourceData(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
}

const geometry::Geometry& MeshAssetSourceData::GetGeometry() const
{
    assert(geometry_ != nullptr && "Geometry data is not set");
    return *geometry_;
}

void MeshAssetSourceData::SetGeometry(std::unique_ptr<geometry::Geometry> geometry)
{
    geometry_ = std::move(geometry);
}

mono_forge_model::MFM& MeshAssetSourceData::GetMFMData() const
{
    assert(mfm_data_ != nullptr && "MFM data is not set");
    return *mfm_data_;
}

void MeshAssetSourceData::SetMFMData(std::unique_ptr<mono_forge_model::MFM> mfm_data)
{
    mfm_data_ = std::move(mfm_data);
}

MeshAssetStagingArea::MeshAssetStagingArea()
{
}

std::unique_ptr<asset_loader::LoadingStagingArea> MeshAssetLoader::Prepare() const
{
    return std::make_unique<MeshAssetStagingArea>();
}

std::unique_ptr<asset_loader::Asset> MeshAssetLoader::Load(
    asset_loader::AssetSourceData& source_data, asset_loader::LoadingStagingArea& staging_area) const
{
    // Cast the source data to the correct type
    MeshAssetSourceData* mesh_source_data = dynamic_cast<MeshAssetSourceData*>(&source_data);
    assert(mesh_source_data != nullptr && "Invalid source data type");

    // Cast the staging area to the correct type
    MeshAssetStagingArea* mesh_staging_area = dynamic_cast<MeshAssetStagingArea*>(&staging_area);
    assert(mesh_staging_area != nullptr && "Invalid staging area type");

    // If file path is set, load file
    if (mesh_source_data->HasFilePath())
    {
        // Get the file path
        const std::string_view file_path = mesh_source_data->GetFilePath();

        // Get file extension
        std::string_view file_extension = utility_header::GetFileExtension(file_path);

        if (file_extension == mono_forge_model::MFM_FILE_EXT)
        {
            // Load MFM file data
            std::unique_ptr<uint8_t[]> mfm_file_data = nullptr;
            fpos_t mfm_file_size = 0;
            mfm_file_data = utility_header::LoadFile(file_path, mfm_file_size);
            if (mfm_file_data == nullptr)
                return nullptr; // Failed to load file

            // Create MFM object
            std::unique_ptr<mono_forge_model::MFM> mfm 
                = std::make_unique<mono_forge_model::MFM>(
                    std::move(mfm_file_data), static_cast<uint32_t>(mfm_file_size));

            // Store the MFM data
            mesh_source_data->SetMFMData(std::move(mfm));
        }
        else
        {
            assert(false && "Unsupported mesh file extension");
            return nullptr; // Unsupported file extension
        }
    }

    std::unique_ptr<MeshAsset> asset = nullptr;
    if (mesh_source_data->HasGeometry())
    {
        // Create vertex date vector
        std::vector<const geometry::Geometry::Vertex*> vertex_data;
        vertex_data.push_back(const_cast<geometry::Geometry::Vertex*>(mesh_source_data->GetGeometry().GetVertexData()));

        // Create vertex count vector
        std::vector<uint32_t> vertex_count;
        vertex_count.push_back(mesh_source_data->GetGeometry().GetVertexCount());

        // Create index data vector
        std::vector<const uint32_t*> index_data;
        index_data.push_back(const_cast<uint32_t*>(mesh_source_data->GetGeometry().GetIndexData()));

        // Create index count vector
        std::vector<uint32_t> index_count;
        index_count.push_back(mesh_source_data->GetGeometry().GetIndexCount());

        // Create the mesh asset instance form the geometry data
        asset = MeshAsset::CreateInstance<MeshAsset>(
            mesh_source_data->GetServiceProxy().Clone(), vertex_data, vertex_count, index_data, index_count);
        assert(asset != nullptr && "Failed to create the mesh asset");
    }
    else if(mesh_source_data->HasMFMData())
    {
        // Get the MFM data
        mono_forge_model::MFM& mfm = mesh_source_data->GetMFMData();

        // Prepare to collect vertex and index data
        std::vector<const geometry::Geometry::Vertex*> vertex_datas;
        std::vector<uint32_t> vertex_counts;
        std::vector<const uint32_t*> index_datas;
        std::vector<uint32_t> index_counts;

        for (uint32_t material_index = 0; material_index < mfm.GetMeshHeader()->material_count; ++material_index)
        {
            // Get mesh node
            const mono_forge_model::MFMMeshNode* mesh_node = mfm.GetMeshNode(material_index);

            // Check vertex size same as geometry vertex size
            assert(
                sizeof(geometry::Geometry::Vertex) == mesh_node->vertex_size && 
                "Vertex size mismatch between Geometry and MFM data");

            // Cast vertex data type to geometry vertex type
            const geometry::Geometry::Vertex* vertex_data 
                = reinterpret_cast<const geometry::Geometry::Vertex*>(mfm.GetVertexData(material_index));

            // Check index size same as geometry index size
            assert(
                sizeof(geometry::Geometry::Index) == mesh_node->index_size && 
                "Index size mismatch between Geometry and MFM data");

            // Cast index data type to geometry index type
            const uint32_t* index_data 
                = reinterpret_cast<const uint32_t*>(mfm.GetIndexData(material_index));

            // Store vertex and index data
            vertex_datas.push_back(vertex_data);
            vertex_counts.push_back(mesh_node->vertex_count);
            index_datas.push_back(index_data);
            index_counts.push_back(mesh_node->index_count);
        }

        // Create the mesh asset instance form the MFM data
        asset = MeshAsset::CreateInstance<MeshAsset>(
            mesh_source_data->GetServiceProxy().Clone(),
            vertex_datas, vertex_counts, index_datas, index_counts);

        // Set asset file path
        asset->SetFilePath(std::string(mesh_source_data->GetFilePath()));
    }
    else
    {
        assert(false && "No valid mesh data found in source data");
    }

    // Set asset name
    if (!mesh_source_data->GetName().empty())
    {
        // Use name from source data if set
        asset->SetName(mesh_source_data->GetName().data());
    }
    else
    {
        // Get file name from file path
        std::string file_name = utility_header::GetFileNameFromPath(mesh_source_data->GetFilePath());
        asset->SetName(file_name);
    }

    return asset; // Return the created asset
}

bool MeshAssetLoader::Commit(asset_loader::LoadingStagingArea& staging_area) const
{
    // Cast the staging area to the correct type
    MeshAssetStagingArea* mesh_staging_area = dynamic_cast<MeshAssetStagingArea*>(&staging_area);
    assert(mesh_staging_area != nullptr && "Invalid staging area type");

    // For this simple mesh loader, there is nothing to commit
    return true;
}

std::unique_ptr<asset_loader::AssetSource> MeshAssetSourceCreator::CreateAssetSource(
    std::wstring_view file_path, mono_service::ServiceProxyManager& service_proxy_manager)
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Create mesh asset source data
    std::unique_ptr<MeshAssetSourceData> asset_source_data
        = std::make_unique<MeshAssetSourceData>(std::move(graphics_service_proxy));

    // Set file path
    asset_source_data->SetFilePath(std::string(file_path.begin(), file_path.end()));

    // Create asset description
    std::unique_ptr<asset_loader::AssetDescription> asset_description
        = std::make_unique<asset_loader::AssetDescription>(
            asset_loader::AssetHandleIDGenerator::GetInstance().Generate(),
            MeshAssetLoader::ID());

    // Create asset source
    std::unique_ptr<asset_loader::AssetSource> asset_source = std::make_unique<asset_loader::AssetSource>();
    asset_source->source_data = std::move(asset_source_data);
    asset_source->description = std::move(asset_description);

    return asset_source; // Success
}

} // namespace mono_asset_extension