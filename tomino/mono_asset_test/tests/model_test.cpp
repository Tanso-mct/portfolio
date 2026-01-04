#include "mono_asset_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_asset/include/model.h"
#pragma comment(lib, "mono_asset.lib")

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_adapter/include/service_adapter.h"

TEST(Model, Create)
{
    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service
    mono_graphics_service::GraphicsService::SetupParam setup_param;
    setup_param.srv_descriptor_count = 50;
    setup_param.rtv_descriptor_count = 50;
    setup_param.dsv_descriptor_count = 50;
    mono_service::ImportService<mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
        *service_registry, 0, setup_param);

    std::unique_ptr<mono_adapter::GraphicsServiceAdapter> graphics_service_adapter = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Initialize service adapter singleton
        graphics_service_adapter = std::make_unique<mono_adapter::GraphicsServiceAdapter>(
            registry.Get(mono_graphics_service::GraphicsServiceHandle::ID()));
    });

    // Async update graphics service
    std::atomic<bool> stop_graphics_service_update = false;
    std::future<void> graphics_service_future = std::async(std::launch::async, [&]()
    {
        bool result = false;

        while (!stop_graphics_service_update.load())
        {
            service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
            {
                // Get graphics service
                mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

                // Pre-update
                result = service.PreUpdate();
                assert(result);

                // Update
                result = service.Update();
                assert(result);

                // Post-update
                result = service.PostUpdate();
                assert(result);
            });
        }
    });

    riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderFBXID());
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load("../resources/mono_asset_test/model/box.mfm");
    ASSERT_NE(fileData, nullptr);

    riaecs::ROObject<riaecs::IAssetFactory> assetFactory = riaecs::gAssetFactoryRegistry->Get(mono_asset::AssetFactoryModelID());
    std::unique_ptr<riaecs::IAssetStagingArea> stagingArea = assetFactory().Prepare();
    std::unique_ptr<riaecs::IAsset> asset = assetFactory().Create(*fileData, *stagingArea);
    assetFactory().Commit(*stagingArea);

    mono_asset::AssetModel &modelAsset = static_cast<mono_asset::AssetModel&>(*asset);

    // Release data which used for loading
    fileData.reset();
    stagingArea.reset();

    // Validate vertex buffer handles
    const std::vector<render_graph::ResourceHandle>* vertex_buffer_handles = modelAsset.GetVertexBufferHandles();
    for (int i = 0; i < vertex_buffer_handles->size(); ++i)
        ASSERT_TRUE(vertex_buffer_handles->at(i).IsValid());

    // Validate index buffer handles
    const std::vector<render_graph::ResourceHandle>* index_buffer_handles = modelAsset.GetIndexBufferHandles();
    for (int i = 0; i < index_buffer_handles->size(); ++i)
        ASSERT_TRUE(index_buffer_handles->at(i).IsValid());

    // Validate index counts
    const std::vector<uint32_t>* index_counts = modelAsset.GetIndexCounts();
    for (int i = 0; i < index_counts->size(); ++i)
        ASSERT_GT(index_counts->at(i), 0u);

    // Finalize async graphics service update
    stop_graphics_service_update.store(true);
    graphics_service_future.get();

    // Clean up
    graphics_service_adapter.reset();
    asset.reset();
    service_registry.reset();
}