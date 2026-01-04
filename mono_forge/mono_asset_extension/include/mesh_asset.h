#pragma once

#include <memory>

#include "class_template/instance.h"
#include "asset_loader/include/asset.h"
#include "asset_loader/include/asset_loader.h"
#include "geometry/include/geometry.h"
#include "mono_forge_model/include/mfm.h"
#include "render_graph/include/resource_handle.h"
#include "mono_service/include/service_registry.h"
#include "mono_asset_extension/include/dll_config.h"
#include "mono_asset_extension/include/asset_source_creator.h"

namespace mono_asset_extension
{

constexpr const char* MESH_ASSET_TYPE_NAME = "Mesh";

// A simple mesh asset class that holds vertex and index buffers
// It uses DirectX 12 buffers for rendering
class MONO_ASSET_EXT_DLL MeshAsset :
    public asset_loader::Asset,
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        MeshAsset,
        class_template::ConstructArgList<std::unique_ptr<mono_service::ServiceProxy>>,
        class_template::SetupArgList<
            const std::vector<const geometry::Geometry::Vertex*>&, const std::vector<uint32_t>&,
            const std::vector<const uint32_t*>&, const std::vector<uint32_t>&>>
{
public:
    MeshAsset(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~MeshAsset() override;
    virtual bool Setup(
        const std::vector<const geometry::Geometry::Vertex*>& vertex_data, const std::vector<uint32_t>& vertex_count,
        const std::vector<const uint32_t*>& index_data, const std::vector<uint32_t>& index_count) override;
    virtual std::string_view GetTypeName() const override { return MESH_ASSET_TYPE_NAME; }

    // Get the handle of the vertex buffer
    const std::vector<render_graph::ResourceHandle>* GetVertexBufferHandles() const;

    // Get the handle of the index buffer
    const std::vector<render_graph::ResourceHandle>* GetIndexBufferHandles() const;

    // Get the number of indices
    const std::vector<uint32_t>* GetIndexCounts() const;

private:
    // Service proxy for asset management
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // Handles for the vertex and index buffers
    std::vector<render_graph::ResourceHandle> vertex_buffer_handles_ = {};

    // Handle for the index buffer
    std::vector<render_graph::ResourceHandle> index_buffer_handles_ = {};

    // Number of indices
    std::vector<uint32_t> index_counts_ = {};
};

// Source data for the MeshAsset
// It holds raw vertex and index data to be used for creating the MeshAsset
class MONO_ASSET_EXT_DLL MeshAssetSourceData :
    public asset_loader::AssetSourceData
{
public:
    MeshAssetSourceData(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~MeshAssetSourceData() override = default;

    // Get the const geometry data
    const geometry::Geometry& GetGeometry() const;

    // Set the geometry data
    void SetGeometry(std::unique_ptr<geometry::Geometry> geometry);

    // Check if geometry data is set
    bool HasGeometry() const { return geometry_ != nullptr; }

    // Get the MFM data
    mono_forge_model::MFM& GetMFMData() const;

    // Set the MFM data
    void SetMFMData(std::unique_ptr<mono_forge_model::MFM> mfm_data);

    // Check if MFM data is set
    bool HasMFMData() const { return mfm_data_ != nullptr; }

    // Get the file path of the mesh asset
    std::string_view GetFilePath() const { return file_path_; }

    // Set the file path of the mesh asset
    void SetFilePath(const std::string& file_path) { file_path_ = file_path; }

    // Check if file path is set
    bool HasFilePath() const { return !file_path_.empty(); }

    // Get the service proxy for asset management
    mono_service::ServiceProxy& GetServiceProxy() { return *graphics_service_proxy_; }

private:
    // Geometry data containing vertex and index information
    std::unique_ptr<geometry::Geometry> geometry_ = nullptr;

    // MFM data for mesh loading
    std::unique_ptr<mono_forge_model::MFM> mfm_data_ = nullptr;

    // File path of the mesh asset
    std::string file_path_ = "";

    // Service proxy for asset management
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;
};

// Staging area for loading the MeshAsset
// Currently, it does not hold any special data
class MONO_ASSET_EXT_DLL MeshAssetStagingArea :
    public asset_loader::LoadingStagingArea
{
public:
    MeshAssetStagingArea();
    virtual ~MeshAssetStagingArea() override = default;

private:
};

// Asset loader for the MeshAsset
// It creates MeshAsset instances from MeshAssetSourceData
class MONO_ASSET_EXT_DLL MeshAssetLoader :
    public asset_loader::AssetLoaderBase<MeshAssetLoader>
{
public:
    MeshAssetLoader() = default;
    virtual ~MeshAssetLoader() override = default;

    std::unique_ptr<asset_loader::LoadingStagingArea> Prepare() const override;
    std::unique_ptr<asset_loader::Asset> Load(
        asset_loader::AssetSourceData& source_data, asset_loader::LoadingStagingArea& staging_area) const override;
    bool Commit(asset_loader::LoadingStagingArea& staging_area) const override;
};

// Asset source creator for the MeshAsset
class MONO_ASSET_EXT_DLL MeshAssetSourceCreator :
    public AssetSourceCreator
{
public:
    virtual ~MeshAssetSourceCreator() override = default;
    virtual std::unique_ptr<asset_loader::AssetSource> CreateAssetSource(
        std::wstring_view file_path, mono_service::ServiceProxyManager& service_proxy_manager) override;
};

} // namespace mono_asset_extension