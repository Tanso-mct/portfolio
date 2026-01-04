#include "mono_render_test/pch.h"
#pragma comment(lib, "riaecs.lib")

#include "mem_alloc_fixed_block/mem_alloc_fixed_block.h"
#pragma comment(lib, "mem_alloc_fixed_block.lib")

#include "mono_render/include/material_handle_manager.h"
#include "mono_render/include/component_camera.h"
#include "mono_render/include/component_mesh_renderer.h"
#include "mono_render/include/directional_light_component.h"
#include "mono_render/include/system_render.h"
#pragma comment(lib, "mono_render.lib")

#include "mono_d3d12/mono_d3d12.h"
#pragma comment(lib, "mono_d3d12.lib")

#include "mono_scene/mono_scene.h"
#pragma comment(lib, "mono_scene.lib")

#include "mono_identity/mono_identity.h"
#pragma comment(lib, "mono_identity.lib")

#include "mono_transform/mono_transform.h"
#pragma comment(lib, "mono_transform.lib")

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

#include "mono_asset/mono_asset.h"
#pragma comment(lib, "mono_asset.lib")

#include "render_graph/include/lambert_material.h"
#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

namespace mono_render_test
{
    constexpr uint32_t BACK_BUFFER_COUNT = 2;
    constexpr uint32_t MAX_LIGHT_COUNT = 10;

    // Create ImGui context
    ImGuiContext* CreateImGuiContextInRender(HWND hwnd)
    {
        bool result = false;

        // Get main monitor scale
        float main_scale 
            = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGuiContext* context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();

        // Bake a fixed style scale. 
        // (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.ScaleAllSizes(main_scale);

        // Set initial font scale. 
        // (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
        style.FontScaleDpi = main_scale;

        // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. 
        // This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleFonts = true;

        // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
        io.ConfigDpiScaleViewports = true;

        // Setup Platform/Renderer backends
        result = ImGui_ImplWin32_Init(hwnd);
        if (!result)
            return nullptr;

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = dx12_util::Device::GetInstance().Get();
        init_info.CommandQueue = dx12_util::CommandQueue::GetInstance().Get();
        init_info.NumFramesInFlight = BACK_BUFFER_COUNT;
        init_info.RTVFormat = render_graph::composition_pass::SWAP_CHAIN_FORMAT;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

        // Get heap manager
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();

        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Get SRV descriptor heap
            dx12_util::DescriptorHeap& srv_heap = heap_manager.GetSrvHeap();

            // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
            init_info.SrvDescriptorHeap = srv_heap.Get();
            init_info.SrvDescriptorAllocFn = [](
                ImGui_ImplDX12_InitInfo*, 
                D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
                {
                    render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                    {
                        dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                        srv_heap_allocator.Allocate(*out_cpu_handle, *out_gpu_handle);
                    });
                };
            init_info.SrvDescriptorFreeFn = [](
                ImGui_ImplDX12_InitInfo*,
                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
                {
                    render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                    {
                        dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                        srv_heap_allocator.Free(cpu_handle, gpu_handle);
                    });
                };
        });

        result = ImGui_ImplDX12_Init(&init_info);
        if (!result)
            return nullptr;
        
        return context;
    }

    // Destroy ImGui context
    void DestroyImguiContextInRender(ImGuiContext* context)
    {
        // Set current ImGui context
        ImGui::SetCurrentContext(context);

        // Shutdown ImGui
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    };

    class TestLambertMaterialHandleID :
        public mono_render::MaterialHandleID<TestLambertMaterialHandleID> {};

    riaecs::AssetSourceRegistrar TestMeshAssetSourceID
    (
        "../resources/mono_render_test/box.mfm",
        mono_file::FileLoaderFBXID(),
        mono_asset::AssetFactoryModelID()
    );

    riaecs::AssetSourceRegistrar EmptyTextureAssetSourceID
    (
        "../resources/mono_render_test/empty_texture.png",
        mono_file::FileLoaderPNGID(),
        mono_asset::AssetFactoryTextureID()
    );

    riaecs::AssetSourceRegistrar TestAlbedoTextureAssetSourceID
    (
        "../resources/mono_render_test/box_albedo.png",
        mono_file::FileLoaderPNGID(),
        mono_asset::AssetFactoryTextureID()
    );

    class TestEntitiesFactory : public mono_scene::IEntitiesFactory
    {
    public:
        riaecs::StagingEntityArea CreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override
        {
            // Get singleton graphics service adapter
            mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
                = mono_adapter::GraphicsServiceAdapter::GetInstance();
                
            // Create graphics service proxy
            std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
                = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

            // Create graphics service command list
            std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list 
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
                = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
            assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

            // Get material handle manager
            mono_render::MaterialHandleManager& material_handle_manager
                = mono_render::MaterialHandleManager::GetInstance();

            riaecs::StagingEntityArea stagingArea = ecsWorld.CreateStagingArea();

            // Create box entity
            {
                riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

                // SceneTag
                ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag* tag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
                    ecsWorld, entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag::SetupParam tagParam;
                tagParam.sceneEntity = sceneEntity;
                tag->Setup(tagParam);

                // Identity
                ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
                    ecsWorld, entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity::SetupParam identityParam;
                identityParam.name = "Box";
                identity->Setup(identityParam);

                // Transform
                ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
                    ecsWorld, entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform::SetupParam transformParam;
                transformParam.scale_ = DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f);
                transform->Setup(transformParam);

                // Create Lambert material setup param
                std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
                    = std::make_unique<render_graph::LambertMaterial::SetupParam>();
                lambert_material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
                lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
                lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
                lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
                lambert_material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

                // Get empty texture asset
                riaecs::ROObject<riaecs::IAsset> emptyTextureAsset 
                    = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
                const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&emptyTextureAsset());
                assert(empty_texture_asset_ptr != nullptr);

                // Get test albedo texture asset
                riaecs::ROObject<riaecs::IAsset> testAlbedoTextureAsset 
                    = assetCont.Get({TestAlbedoTextureAssetSourceID(), assetCont.GetGeneration(TestAlbedoTextureAssetSourceID())});
                const mono_asset::AssetTexture* test_albedo_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&testAlbedoTextureAsset());
                assert(test_albedo_texture_asset_ptr != nullptr);

                lambert_material_param->albedo_texture_handle = test_albedo_texture_asset_ptr->GetTextureHandle();
                lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
                lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
                lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

                // Register material handle
                material_handle_manager.RegisterMaterialHandle(TestLambertMaterialHandleID().ID());

                // Create Lambert material
                graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
                    material_handle_manager.GetMaterialHandle(TestLambertMaterialHandleID().ID()),
                    std::move(lambert_material_param));

                // Get test mesh asset
                riaecs::ROObject<riaecs::IAsset> meshAsset 
                    = assetCont.Get({TestMeshAssetSourceID(), assetCont.GetGeneration(TestMeshAssetSourceID())});
                const mono_asset::AssetModel* modelAsset = dynamic_cast<const mono_asset::AssetModel*>(&meshAsset());
                assert(modelAsset != nullptr);

                // MeshRenderer
                ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
                mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
                    ecsWorld, entity, mono_render::ComponentMeshRendererID());

                mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
                meshRendererParam.vertex_buffer_handles = *modelAsset->GetVertexBufferHandles();
                meshRendererParam.index_buffer_handles = *modelAsset->GetIndexBufferHandles();
                meshRendererParam.index_counts = *modelAsset->GetIndexCounts();

                std::vector<const render_graph::MaterialHandle*> material_handles;
                material_handles.resize(modelAsset->GetIndexCounts()->size());
                for (int i = 0; i < material_handles.size(); ++i)
                    material_handles[i] = material_handle_manager.GetMaterialHandle(TestLambertMaterialHandleID().ID());
                meshRendererParam.material_handles = std::move(material_handles);

                meshRenderer->Setup(meshRendererParam);
            }

            // Create camera entity
            {
                riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

                // SceneTag
                ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
                    ecsWorld, entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
                sceneTagParam.sceneEntity = sceneEntity;
                sceneTag->Setup(sceneTagParam);

                // Identity
                ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
                    ecsWorld, entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity::SetupParam identityParam;
                identityParam.name = "Camera";
                identity->Setup(identityParam);

                // Transform
                ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
                    ecsWorld, entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform::SetupParam transformParam;
                transformParam.pos_ = DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f);
                transform->Setup(transformParam);

                // Camera
                ecsWorld.AddComponent(entity, mono_render::ComponentCameraID());
                mono_render::ComponentCamera* camera = riaecs::GetComponent<mono_render::ComponentCamera>(
                    ecsWorld, entity, mono_render::ComponentCameraID());
                mono_render::ComponentCamera::SetupParam cameraParam;
                cameraParam.fov_y = 60.0f;
                cameraParam.nearZ = 0.1f;
                cameraParam.farZ = 1000.0f;
                cameraParam.aspect_ratio = 16.0f / 9.0f;
                camera->Setup(cameraParam);
            }

            // Create directional light entity
            {
                riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

                // SceneTag
                ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
                    ecsWorld, entity, mono_scene::ComponentSceneTagID());
                mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
                sceneTagParam.sceneEntity = sceneEntity;
                sceneTag->Setup(sceneTagParam);

                // Identity
                ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
                    ecsWorld, entity, mono_identity::ComponentIdentityID());
                mono_identity::ComponentIdentity::SetupParam identityParam;
                identityParam.name = "Directional Light";
                identity->Setup(identityParam);

                // Transform
                ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
                    ecsWorld, entity, mono_transform::ComponentTransformID());
                mono_transform::ComponentTransform::SetupParam transformParam;
                transformParam.pitch_ = 60.0f;
                transformParam.yaw_ = 90.0f;
                transform->Setup(transformParam);

                // Directional Light
                ecsWorld.AddComponent(entity, mono_render::DirectionalLightComponentID());
                mono_render::DirectionalLightComponent* directionalLight = riaecs::GetComponent<mono_render::DirectionalLightComponent>(
                    ecsWorld, entity, mono_render::DirectionalLightComponentID());
                mono_render::DirectionalLightComponent::SetupParam lightParam;
                directionalLight->Setup(lightParam);
            }

            // Submit command list
            graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

            return stagingArea;
        }
        
    };

} // namespace mono_render_test

TEST(Render, Execute)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create ECS World
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create Asset Container
    std::unique_ptr<riaecs::IAssetContainer> assetCont = std::make_unique<riaecs::AssetContainer>();
    assetCont->Create(riaecs::gAssetSourceRegistry->GetCount());

    // Create window system
    mono_d3d12::SystemWindowD3D12 windowSystem;

    // Create render  system
    mono_render::SystemRender renderSystem;

    // Create System Loop Command Queue
    std::unique_ptr<riaecs::ISystemLoopCommandQueue> systemLoopCmdQueue = std::make_unique<riaecs::SystemLoopCommandQueue>();

    /*******************************************************************************************************************
     * Run the main loop to process messages
    /******************************************************************************************************************/

    bool isRunning = true;
    std::future<void> gameLoop = std::async(std::launch::async, 
        [&isRunning, &windowSystem, &renderSystem, &ecsWorld, &assetCont, &systemLoopCmdQueue]()
    {
        while (isRunning)
        {
            // Update window system
            isRunning = windowSystem.Update(*ecsWorld, *assetCont, *systemLoopCmdQueue);
            if (!isRunning)
                break;

            // Update render system
            isRunning = renderSystem.Update(*ecsWorld, *assetCont, *systemLoopCmdQueue);
            if (!isRunning)
                break;
        }
    });

    // Create staging entity area
    riaecs::StagingEntityArea entityStagingArea = ecsWorld->CreateStagingArea();

    // Create scene
    riaecs::Entity sceneEntity;
    {
        sceneEntity = ecsWorld->CreateEntity(entityStagingArea);

        // Identity
        ecsWorld->AddComponent(sceneEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            *ecsWorld, sceneEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "Scene";
        identity->Setup(identityParam);

        // Scene
        sceneEntity = ecsWorld->CreateEntity();
        ecsWorld->AddComponent(sceneEntity, mono_scene::ComponentSceneID());
        mono_scene::ComponentScene* scene = riaecs::GetComponent<mono_scene::ComponentScene>(
            *ecsWorld, sceneEntity, mono_scene::ComponentSceneID());
        mono_scene::ComponentScene::SetupParam param;
        param.entitiesFactory_ = std::make_unique<mono_render_test::TestEntitiesFactory>();
        param.assetSourceIDs_.push_back(mono_render_test::TestMeshAssetSourceID());
        param.assetSourceIDs_.push_back(mono_render_test::EmptyTextureAssetSourceID());
        param.assetSourceIDs_.push_back(mono_render_test::TestAlbedoTextureAssetSourceID());
        param.needsLoad_ = true;
        param.needsEditSystemList_ = false;
        scene->Setup(param);
        mono_scene::LoadScene(sceneEntity, scene, *ecsWorld, *assetCont);
        ecsWorld->CommitEntities(scene->StagingEntityAreaRW()());
    }

    // Create and show a D3D12 Window
    {
        riaecs::Entity entity = ecsWorld->CreateEntity(entityStagingArea);

        // Identity
        ecsWorld->AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            *ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "Window";
        identity->Setup(identityParam);

        // Window
        ecsWorld->AddComponent(entity, mono_d3d12::ComponentWindowD3D12ID());
        mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponent<mono_d3d12::ComponentWindowD3D12>(
            *ecsWorld, entity, mono_d3d12::ComponentWindowD3D12ID());
        mono_d3d12::ComponentWindowD3D12::SetupParam windowParam;
        windowParam.windowName_ = L"Render Test";
        windowParam.windowClassName_ = L"RenderTestWindowClass";
        windowParam.sceneEntity_ = sceneEntity;
        windowParam.needsQuitAppWhenDestroyed_ = true;
        window->Setup(windowParam);
        window->Create();
        window->Show();

        // Window Render Bind
        ecsWorld->AddComponent(entity, mono_d3d12::WindowRenderBindComponentID());

        mono_d3d12::WindowRenderBindComponent* windowRenderBind
        = riaecs::GetComponent<mono_d3d12::WindowRenderBindComponent>(*ecsWorld, entity, mono_d3d12::WindowRenderBindComponentID());

        mono_d3d12::WindowRenderBindComponent::SetupParam renderBindSetupParam;
        renderBindSetupParam.imgui_create_func = mono_render_test::CreateImGuiContextInRender;
        renderBindSetupParam.imgui_destroy_func = mono_render_test::DestroyImguiContextInRender;
        windowRenderBind->Setup(renderBindSetupParam);
    }

    // Commit created entities
    ecsWorld->CommitEntities(entityStagingArea);

    /*******************************************************************************************************************
     * Run the message loop to keep the window responsive
    /******************************************************************************************************************/

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!isRunning)
            PostQuitMessage(0);
    }

    // Wait for the main loop to finish
    gameLoop.get();

    // Clean up
    ecsWorld.reset();
    assetCont.reset();

    CoUninitialize();
}