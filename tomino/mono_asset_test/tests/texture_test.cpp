#include "mono_asset_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_asset/include/texture.h"
#pragma comment(lib, "mono_asset.lib")

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_adapter/include/service_adapter.h"

TEST(Texture, Create)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

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

    riaecs::ROObject<riaecs::IFileLoader> file_loader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderPNGID());
    std::unique_ptr<riaecs::IFileData> file_data = file_loader().Load("../resources/mono_asset_test/texture/Lenna.png");
    ASSERT_NE(file_data, nullptr);

    riaecs::ROObject<riaecs::IAssetFactory> asset_factory = riaecs::gAssetFactoryRegistry->Get(mono_asset::AssetFactoryTextureID());
    std::unique_ptr<riaecs::IAssetStagingArea> staging_area = asset_factory().Prepare();
    std::unique_ptr<riaecs::IAsset> asset = asset_factory().Create(*file_data, *staging_area);
    asset_factory().Commit(*staging_area);

    // Release data which used for loading
    file_data.reset();
    staging_area.reset();

    // Get texture asset
    mono_asset::AssetTexture &texture_asset = static_cast<mono_asset::AssetTexture&>(*asset);

    // Validate texture 2d handle
    const render_graph::ResourceHandle* texture_handle = texture_asset.GetTextureHandle();
    ASSERT_NE(texture_handle, nullptr);

    // Validate upload buffer handle
    const render_graph::ResourceHandle* upload_buffer_handle = texture_asset.GetUploadBufferHandle();
    ASSERT_NE(upload_buffer_handle, nullptr);

    // Finalize async graphics service update
    stop_graphics_service_update.store(true);
    graphics_service_future.get();

    // Clean up
    graphics_service_adapter.reset();
    asset.reset();
    service_registry.reset();

    CoUninitialize();
}