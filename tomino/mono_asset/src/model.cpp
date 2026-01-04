#include "mono_asset/src/pch.h"
#include "mono_asset/include/model.h"

#include "geometry/include/geometry.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

#pragma comment(lib, "riaecs.lib")

using namespace DirectX;

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

mono_asset::AssetModel::AssetModel(
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null");
}

mono_asset::AssetModel::~AssetModel()
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

const std::vector<render_graph::ResourceHandle>* mono_asset::AssetModel::GetVertexBufferHandles() const
{
    for (const auto& handle : vertex_buffer_handles_)
        assert(handle.IsValid() && "Vertex buffer handle is not valid");

    return &vertex_buffer_handles_;
}

std::vector<render_graph::ResourceHandle>* mono_asset::AssetModel::GetVertexBufferHandles()
{
    return &vertex_buffer_handles_;
}

const std::vector<render_graph::ResourceHandle>* mono_asset::AssetModel::GetIndexBufferHandles() const
{
    for (const auto& handle : index_buffer_handles_)
        assert(handle.IsValid() && "Index buffer handle is not valid");

    return &index_buffer_handles_;
}

std::vector<render_graph::ResourceHandle>* mono_asset::AssetModel::GetIndexBufferHandles()
{
    return &index_buffer_handles_;
}

const std::vector<uint32_t>* mono_asset::AssetModel::GetIndexCounts() const
{
    return &index_counts_;
}

std::vector<uint32_t>* mono_asset::AssetModel::GetIndexCounts()
{
    return &index_counts_;
}

mono_asset::AssetStagingAreaModel::AssetStagingAreaModel(
    std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list) :
    graphics_command_list_(std::move(graphics_command_list))
{
    assert(graphics_command_list_ != nullptr && "Graphics service command list is null");
}

mono_asset::AssetStagingAreaModel::~AssetStagingAreaModel()
{
}

mono_asset::AssetFactoryModel::AssetFactoryModel()
{
}

mono_asset::AssetFactoryModel::~AssetFactoryModel()
{
}

std::unique_ptr<riaecs::IAssetStagingArea> mono_asset::AssetFactoryModel::Prepare() const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy->CreateCommandList();

    return std::make_unique<AssetStagingAreaModel>(std::move(command_list));
}

std::unique_ptr<riaecs::IAsset> mono_asset::AssetFactoryModel::Create(
    const riaecs::IFileData &file_data, riaecs::IAssetStagingArea &staging_area) const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Get FBX file data
    const mono_file::FileDataFBX* fbx_data = dynamic_cast<const mono_file::FileDataFBX*>(&file_data);
    assert(fbx_data && "Invalid file data type for FBX");

    // Get fbx staging area
    mono_asset::AssetStagingAreaModel* model_staging_area
        = dynamic_cast<mono_asset::AssetStagingAreaModel*>(&staging_area);
    assert(model_staging_area && "Invalid staging area type for model");

    // Get graphics service command list
    mono_service::ServiceCommandList& graphics_command_list
        = model_staging_area->GetGraphicsServiceCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(&graphics_command_list);
    assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

    // Create model asset
    std::unique_ptr<AssetModel> model_asset = std::make_unique<AssetModel>(
        graphics_service_proxy->Clone());

    // Get the MFM data
    const mono_forge_model::MFM& mfm = fbx_data->GetMFM();

    // Prepare to collect vertex and index data
    std::vector<const geometry::Geometry::Vertex*> vertex_datas;
    std::vector<uint32_t> vertex_counts;
    std::vector<const uint32_t*> index_datas;

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
        model_asset->GetIndexCounts()->push_back(mesh_node->index_count);
    }

    // Resize handle vectors
    model_asset->GetVertexBufferHandles()->resize(vertex_datas.size());
    model_asset->GetIndexBufferHandles()->resize(index_datas.size());

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
    for (size_t i = 0; i < vertex_datas.size(); ++i)
    {
        const render_graph::geometry_pass::Vertex* vertex_data_rg
            = reinterpret_cast<const render_graph::geometry_pass::Vertex*>(vertex_datas[i]);
        graphics_command_list_ptr->CreateVertexBuffer(
            &model_asset->GetVertexBufferHandles()->at(i), vertex_counts[i], vertex_data_rg);
    }

    // Create index buffer
    for (size_t i = 0; i < index_datas.size(); ++i)
    {
        const uint32_t* index_data_rg = reinterpret_cast<const uint32_t*>(index_datas[i]);
        graphics_command_list_ptr->CreateIndexBuffer(
            &model_asset->GetIndexBufferHandles()->at(i), model_asset->GetIndexCounts()->at(i), index_data_rg);
    }

    return model_asset;
}

void mono_asset::AssetFactoryModel::Commit(riaecs::IAssetStagingArea &staging_area) const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();

    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Get model staging area
    mono_asset::AssetStagingAreaModel* model_staging_area
        = dynamic_cast<mono_asset::AssetStagingAreaModel*>(&staging_area);
    assert(model_staging_area && "Invalid staging area type for model");

    // Get graphics service command list
    mono_service::ServiceCommandList& graphics_command_list
        = model_staging_area->GetGraphicsServiceCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(&graphics_command_list);
    assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

    // Submit command list to graphics service
    mono_service::ServiceProgress progress 
        = graphics_service_proxy->SubmitCommandList(model_staging_area->TakeGraphicsServiceCommandList());

    // Wait for the graphics service to complete the submitted commands
    const int32_t progress_completion_offset = 1;
    while (graphics_service_proxy->GetProgress() <= progress + progress_completion_offset)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

MONO_ASSET_API riaecs::AssetFactoryRegistrar<mono_asset::AssetFactoryModel> mono_asset::AssetFactoryModelID;