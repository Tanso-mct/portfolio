#pragma once
#include "mono_asset/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_service/include/service_registry.h"
#include "render_graph/include/resource_handle.h"

#include <vector>

namespace mono_asset
{
    class MONO_ASSET_API AssetModel : public riaecs::IAsset
    {
    private:
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;
    
        // Handles for the vertex and index buffers
        std::vector<render_graph::ResourceHandle> vertex_buffer_handles_ = {};

        // Handle for the index buffer
        std::vector<render_graph::ResourceHandle> index_buffer_handles_ = {};

        // Number of indices
        std::vector<uint32_t> index_counts_ = {};

    public:
        AssetModel(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
        ~AssetModel() override;

        // Get the handle of the vertex buffer const
        const std::vector<render_graph::ResourceHandle>* GetVertexBufferHandles() const;

        // Get the handle of the vertex buffer
        std::vector<render_graph::ResourceHandle>* GetVertexBufferHandles();

        // Get the handle of the index buffer const
        const std::vector<render_graph::ResourceHandle>* GetIndexBufferHandles() const;

        // Get the handle of the index buffer
        std::vector<render_graph::ResourceHandle>* GetIndexBufferHandles();

        // Get the number of indices const
        const std::vector<uint32_t>* GetIndexCounts() const;

        // Get the number of indices
        std::vector<uint32_t>* GetIndexCounts();
    };

    class MONO_ASSET_API AssetStagingAreaModel : public riaecs::IAssetStagingArea
    {
    private:
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list_ = nullptr;

    public:
        AssetStagingAreaModel(std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list);
        ~AssetStagingAreaModel() override;

        // Get the graphics service command list
        mono_service::ServiceCommandList& GetGraphicsServiceCommandList()
        {
            assert(graphics_command_list_ != nullptr && "Graphics service command list is null");
            return *graphics_command_list_;
        }

        // Take ownership of the graphics service command list
        std::unique_ptr<mono_service::ServiceCommandList> TakeGraphicsServiceCommandList()
        {
            return std::move(graphics_command_list_);
        }
    };

    class MONO_ASSET_API AssetFactoryModel : public riaecs::IAssetFactory
    {
    public:
        AssetFactoryModel();
        ~AssetFactoryModel();

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override;
        std::unique_ptr<riaecs::IAsset> Create(
            const riaecs::IFileData &file_data, riaecs::IAssetStagingArea &staging_area) const override;
        void Commit(riaecs::IAssetStagingArea &staging_area) const override;
    };
    extern MONO_ASSET_API riaecs::AssetFactoryRegistrar<AssetFactoryModel> AssetFactoryModelID;

} // namespace mono_asset