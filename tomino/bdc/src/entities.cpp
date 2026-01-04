#include "bdc/src/pch.h"
#include "bdc/include/entities.h"

#include "bdc/include/asset_source.h"
#include "bdc/include/materials.h"

#include "bdc/include/scene_menu.h"
#include "bdc/include/scene_play.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_d3d12.lib")
#pragma comment(lib, "mono_scene.lib")
#pragma comment(lib, "mono_render.lib")
#pragma comment(lib, "mono_physics.lib")
#pragma comment(lib, "mono_file.lib")

#pragma comment(lib, "mono_object_controller.lib")
#pragma comment(lib, "bdc_player.lib")
#pragma comment(lib, "bdc_dot.lib")
#pragma comment(lib, "bdc_game_flow.lib")
#pragma comment(lib, "bdc_cage.lib")
#pragma comment(lib, "bdc_ui.lib")
#pragma comment(lib, "bdc_interactive_object.lib")

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_render/include/material_handle_manager.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

using namespace DirectX;

namespace bdc
{

// Create ImGui context
ImGuiContext* CreateImGuiContext(HWND hwnd)
{
    bool result = false;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    float font_size = 16.0f;
    
    ImFont* font 
        = io.Fonts->AddFontFromFileTTF(
            "../resources/bdc/YokohamaDotsJPN.otf", font_size, nullptr, io.Fonts->GetGlyphRangesJapanese());
    io.FontDefault = font;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

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

    // Set global handling imGui context pointer
    mono_d3d12::g_HandleImguiContext = context;
    
    return context;
}

// Destroy ImGui context
void DestroyImguiContext(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    // Shutdown ImGui
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
};

} // namespace bdc

riaecs::Entity bdc::CreateMenuScene(riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont)
{
    riaecs::Entity entity = ecsWorld.CreateEntity();

    // Register entity
    ecsWorld.RegisterEntity(size_t(bdc::EntityRegisterIndex::MENU_SCENE), entity);

    // Identity component
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "MenuScene";
    identity->Setup(identityParam);

    // Scene component
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneID());
    mono_scene::ComponentScene* scene = riaecs::GetComponent<mono_scene::ComponentScene>(
        ecsWorld, entity, mono_scene::ComponentSceneID());
    mono_scene::ComponentScene::SetupParam sceneParam;
    sceneParam.entitiesFactory_ = std::make_unique<bdc::MenuSceneEntitiesFactory>();
    sceneParam.assetSourceIDs_ = 
    { 
        // Empty
        bdc::EmptyTextureAssetSourceID(),

        // Menu Background
        bdc::MenuBackgroundTextureAssetSourceID(),

        // Loading Background
        bdc::LoadingBackgroundTextureAssetSourceID(),
    };
    sceneParam.systemListEditCmds_.push_back(std::make_unique<bdc::MenuSceneSystemListEditCmd>());
    sceneParam.targetEditCmdIndex_ = 0;
    sceneParam.needsLoad_ = true;
    sceneParam.needsEditSystemList_ = true;
    sceneParam.clear_color_ = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // White
    scene->Setup(sceneParam);

    return entity;
}

riaecs::Entity bdc::CreatePlayScene(riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont)
{
    riaecs::Entity entity = ecsWorld.CreateEntity();

    // Register entity
    ecsWorld.RegisterEntity(size_t(bdc::EntityRegisterIndex::PLAY_SCENE), entity);

    // Identity component
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "PlayScene";
    identity->Setup(identityParam);

    // Scene component
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneID());
    mono_scene::ComponentScene* scene = riaecs::GetComponent<mono_scene::ComponentScene>(
        ecsWorld, entity, mono_scene::ComponentSceneID());
    mono_scene::ComponentScene::SetupParam sceneParam;
    sceneParam.entitiesFactory_ = std::make_unique<bdc::PlaySceneEntitiesFactory>();
    sceneParam.assetSourceIDs_ = 
    { 
        // Empty
        bdc::EmptyTextureAssetSourceID(),

        // Player
        bdc::PlayerModelAssetSourceID(), bdc::PlayerAlbedoTextureAssetSourceID(),
        bdc::PlayerLocatorAssetSourceID(), bdc::PlayerBoundingBoxAssetSourceID(),

        // Enhanced Player
        bdc::EnhancedPlayerAlbedoTextureAssetSourceID(),

        // Floor
        bdc::FloorModelAssetSourceID(), bdc::FloorAlbedoTextureAssetSourceID(),
        bdc::FloorLocatorAssetSourceID(),

        // BookShelf
        bdc::BookShelfModelAssetSourceID(), bdc::BookShelfAlbedoTextureAssetSourceID(),
        bdc::BookShelfLocatorAssetSourceID(), bdc::BookShelfBoundingBoxAssetSourceID(),

        // CatScratch
        bdc::CatScratchModelAssetSourceID(), bdc::CatScratchAlbedoTextureAssetSourceID(),
        bdc::CatScratchLocatorAssetSourceID(), bdc::CatScratchBoundingBoxAssetSourceID(),

        // FoodShelf01
        bdc::FoodShelf01ModelAssetSourceID(), bdc::FoodShelf01AlbedoTextureAssetSourceID(),
        bdc::FoodShelf01LocatorAssetSourceID(), bdc::FoodShelf01BoundingBoxAssetSourceID(),

        // FoodShelf02
        bdc::FoodShelf02ModelAssetSourceID(), bdc::FoodShelf02AlbedoTextureAssetSourceID(),
        bdc::FoodShelf02LocatorAssetSourceID(), bdc::FoodShelf02BoundingBoxAssetSourceID(),

        // Table4x20
        bdc::Table4x20ModelAssetSourceID(), bdc::Table4x20AlbedoTextureAssetSourceID(),
        bdc::Table4x20LocatorAssetSourceID(), bdc::Table4x20BoundingBoxAssetSourceID(),

        // Table4x6
        bdc::Table4x6ModelAssetSourceID(), bdc::Table4x6AlbedoTextureAssetSourceID(),
        bdc::Table4x6LocatorAssetSourceID(), bdc::Table4x6BoundingBoxAssetSourceID(),

        // Table6x8
        bdc::Table6x8ModelAssetSourceID(), bdc::Table6x8AlbedoTextureAssetSourceID(),
        bdc::Table6x8LocatorAssetSourceID(), bdc::Table6x8BoundingBoxAssetSourceID(),

        // Table4x28
        bdc::Table4x28ModelAssetSourceID(), bdc::Table4x28AlbedoTextureAssetSourceID(),
        bdc::Table4x28LocatorAssetSourceID(), bdc::Table4x28BoundingBoxAssetSourceID(),

        // Wall1
        bdc::Wall1ModelAssetSourceID(), bdc::Wall1AlbedoTextureAssetSourceID(),
        bdc::Wall1LocatorAssetSourceID(), bdc::Wall1BoundingBoxAssetSourceID(),

        // Wall2
        bdc::Wall2ModelAssetSourceID(), bdc::Wall2AlbedoTextureAssetSourceID(),
        bdc::Wall2LocatorAssetSourceID(), bdc::Wall2BoundingBoxAssetSourceID(),

        // Refrigerator
        bdc::RefrigeratorModelAssetSourceID(),
        bdc::RefrigeratorLocatorAssetSourceID(), bdc::RefrigeratorBoundingBoxAssetSourceID(),

        // Kitchen
        bdc::KitchenModelAssetSourceID(), bdc::KitchenAlbedoTextureAssetSourceID(),
        bdc::KitchenLocatorAssetSourceID(), bdc::KitchenBoundingBoxAssetSourceID(),

        // Chair2x2
        bdc::Chair2x2ModelAssetSourceID(), bdc::Chair2x2AlbedoTextureAssetSourceID(),
        bdc::Chair2x2LocatorAssetSourceID(), bdc::Chair2x2BoundingBoxAssetSourceID(),

        // Chair2x4
        bdc::Chair2x4ModelAssetSourceID(), bdc::Chair2x4AlbedoTextureAssetSourceID(),
        bdc::Chair2x4LocatorAssetSourceID(), bdc::Chair2x4BoundingBoxAssetSourceID(),

        // Sofa
        bdc::SofaModelAssetSourceID(), bdc::SofaAlbedoTextureAssetSourceID(),
        bdc::SofaLocatorAssetSourceID(), bdc::SofaBoundingBoxAssetSourceID(),

        // Mouse trap
        bdc::MouseTrapModelAssetSourceID(), bdc::MouseTrapAlbedoTextureAssetSourceID(),
        bdc::MouseTrapLocatorAssetSourceID(), bdc::MouseTrapBoundingBoxAssetSourceID(),

        // Wood box texture
        bdc::WoodBoxAlbedoTextureAssetSourceID(),

        // Wood boxes
        bdc::WoodBox01ModelAssetSourceID(), bdc::WoodBox01LocatorAssetSourceID(), bdc::WoodBox01BoundingBoxAssetSourceID(),
        bdc::WoodBox02ModelAssetSourceID(), bdc::WoodBox02LocatorAssetSourceID(), bdc::WoodBox02BoundingBoxAssetSourceID(),
        bdc::WoodBox03ModelAssetSourceID(), bdc::WoodBox03LocatorAssetSourceID(), bdc::WoodBox03BoundingBoxAssetSourceID(),
        bdc::WoodBox04ModelAssetSourceID(), bdc::WoodBox04LocatorAssetSourceID(), bdc::WoodBox04BoundingBoxAssetSourceID(),
        bdc::WoodBox05ModelAssetSourceID(), bdc::WoodBox05LocatorAssetSourceID(), bdc::WoodBox05BoundingBoxAssetSourceID(),
        bdc::WoodBox06ModelAssetSourceID(), bdc::WoodBox06LocatorAssetSourceID(), bdc::WoodBox06BoundingBoxAssetSourceID(),
        bdc::WoodBox07ModelAssetSourceID(), bdc::WoodBox07LocatorAssetSourceID(), bdc::WoodBox07BoundingBoxAssetSourceID(),
        bdc::WoodBox08ModelAssetSourceID(), bdc::WoodBox08LocatorAssetSourceID(), bdc::WoodBox08BoundingBoxAssetSourceID(),
        bdc::WoodBox09ModelAssetSourceID(), bdc::WoodBox09LocatorAssetSourceID(), bdc::WoodBox09BoundingBoxAssetSourceID(),
        bdc::WoodBox10ModelAssetSourceID(), bdc::WoodBox10LocatorAssetSourceID(), bdc::WoodBox10BoundingBoxAssetSourceID(),
        bdc::WoodBox11ModelAssetSourceID(), bdc::WoodBox11LocatorAssetSourceID(), bdc::WoodBox11BoundingBoxAssetSourceID(),
        bdc::WoodBox12ModelAssetSourceID(), bdc::WoodBox12LocatorAssetSourceID(), bdc::WoodBox12BoundingBoxAssetSourceID(),

        // Sofa table
        bdc::SofaTableModelAssetSourceID(), bdc::SofaTableAlbedoTextureAssetSourceID(),
        bdc::SofaTableLocatorAssetSourceID(), bdc::SofaTableBoundingBoxAssetSourceID(),

        // Dot
        bdc::DotModelAssetSourceID(),
        bdc::DotLocatorAssetSourceID(), bdc::DotBoundingBoxAssetSourceID(),

        // Large Enemy
        bdc::LargeEnemyModelAssetSourceID(), bdc::LargeEnemyBoundingBoxAssetSourceID(),

        // Small Enemy
        bdc::SmallEnemyModelAssetSourceID(), bdc::SmallEnemyBoundingBoxAssetSourceID(),

        // Cage
        bdc::CageModelAssetSourceID(),
        bdc::CageLocatorAssetSourceID(), bdc::NorthCageBoundingBoxAssetSourceID(),
        bdc::SouthCageBoundingBoxAssetSourceID(), bdc::WestCageBoundingBoxAssetSourceID(),
        bdc::EastCageBoundingBoxAssetSourceID(),

        // Crystal
        bdc::CrystalModelAssetSourceID(), bdc::CrystalBoundingBoxAssetSourceID(),

        // Loading Background
        bdc::LoadingBackgroundTextureAssetSourceID(),

        // Player Bar
        bdc::PlayerBarTextureAssetSourceID(),

        // Game Over Background
        bdc::GameOverBackgroundTextureAssetSourceID(),
    };
    sceneParam.systemListEditCmds_.push_back(std::make_unique<bdc::PlaySceneSystemListEditCmd>());
    sceneParam.targetEditCmdIndex_ = 0;
    sceneParam.needsLoad_ = false;
    sceneParam.needsEditSystemList_ = true;
    scene->Setup(sceneParam);

    return entity;
}

riaecs::Entity bdc::CreateMainWindow(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, const riaecs::Entity &sceneEntity)
{
    riaecs::Entity entity = ecsWorld.CreateEntity();
    ecsWorld.RegisterEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW, entity);

    // Identity component
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "GameWindow";
    identity->Setup(identityParam);

    // Window component
    ecsWorld.AddComponent(entity, mono_d3d12::ComponentWindowD3D12ID());
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponent<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, entity, mono_d3d12::ComponentWindowD3D12ID());
    mono_d3d12::ComponentWindowD3D12::SetupParam windowParam;
    windowParam.windowName_ = L"BDC Game";
    windowParam.windowClassName_ = L"BDCGameWindowClass";
    windowParam.windowWidth_ = 1920;
    windowParam.windowHeight_ = 1080;
    windowParam.needsQuitAppWhenDestroyed_ = true;
    window->Setup(windowParam);
    window->Create();
    window->Show();

    // Window Render Bind
    ecsWorld.AddComponent(entity, mono_d3d12::WindowRenderBindComponentID());
    mono_d3d12::WindowRenderBindComponent* windowRenderBind
        = riaecs::GetComponent<mono_d3d12::WindowRenderBindComponent>(ecsWorld, entity, mono_d3d12::WindowRenderBindComponentID());

    mono_d3d12::WindowRenderBindComponent::SetupParam renderBindSetupParam;
    renderBindSetupParam.imgui_create_func = bdc::CreateImGuiContext;
    renderBindSetupParam.imgui_destroy_func = bdc::DestroyImguiContext;
    windowRenderBind->Setup(renderBindSetupParam);

    return entity;
}

riaecs::Entity bdc::menu_scene::CreateMainCamera
(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const nlohmann::json& configJson
){
    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = menuSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "MainCamera";
    identity->Setup(identityParam);

    // Transform
    ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
        ecsWorld, entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform::SetupParam transformParam;
    transform->Setup(transformParam);

    // Camera
    ecsWorld.AddComponent(entity, mono_render::ComponentCameraID());
    mono_render::ComponentCamera* camera = riaecs::GetComponent<mono_render::ComponentCamera>(
        ecsWorld, entity, mono_render::ComponentCameraID());
    mono_render::ComponentCamera::SetupParam cameraParam;
    cameraParam.fov_y = 90.0f;
    cameraParam.nearZ = 0.1f;
    cameraParam.farZ = 1000.0f;
    cameraParam.aspect_ratio = 16.0f / 9.0f;
    camera->Setup(cameraParam);

    return entity;
}

riaecs::Entity bdc::menu_scene::CreateMenuTitle
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const nlohmann::json& configJson
){
    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = menuSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "MenuBackground";
    identity->Setup(identityParam);

    // UI
    ecsWorld.AddComponent(entity, mono_render::UIComponentID());
    mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
        ecsWorld, entity, mono_render::UIComponentID());
    mono_render::UIComponent::SetupParam uiParam;
    uiParam.ui_drawer = std::make_unique<bdc_ui::ImageUIDrawer>(
        window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
        configJson["menu_ui"]["title"]["position"]["x"].get<float>(),
        configJson["menu_ui"]["title"]["position"]["y"].get<float>(),
        configJson["menu_ui"]["title"]["size"]["x"].get<float>(),
        configJson["menu_ui"]["title"]["size"]["y"].get<float>(),
        bdc::MenuBackgroundTextureAssetSourceID(), assetCont, "MenuBackground");
    uiComponent->Setup(uiParam);

    return entity;
}

riaecs::Entity bdc::menu_scene::CreateStartButton
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const nlohmann::json& configJson,
    const riaecs::Entity& loadingBackgroundEntity
){
    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = menuSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "StartButton";
    identity->Setup(identityParam);

    // UI
    ecsWorld.AddComponent(entity, mono_render::UIComponentID());
    mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
        ecsWorld, entity, mono_render::UIComponentID());
    mono_render::UIComponent::SetupParam uiComponentParam;
    uiComponentParam.ui_drawer = std::make_unique<bdc_ui::ButtonUIDrawer>(
        window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
        configJson["menu_ui"]["start_button"]["text"].get<std::string>(), 
        configJson["menu_ui"]["start_button"]["text_size"].get<float>(),
        DirectX::XMINT4(
            configJson["menu_ui"]["start_button"]["text_color"]["r"].get<int>(),
            configJson["menu_ui"]["start_button"]["text_color"]["g"].get<int>(),
            configJson["menu_ui"]["start_button"]["text_color"]["b"].get<int>(),
            configJson["menu_ui"]["start_button"]["text_color"]["a"].get<int>()),
        DirectX::XMINT4(
            configJson["menu_ui"]["start_button"]["hover_text_color"]["r"].get<int>(),
            configJson["menu_ui"]["start_button"]["hover_text_color"]["g"].get<int>(),
            configJson["menu_ui"]["start_button"]["hover_text_color"]["b"].get<int>(),
            configJson["menu_ui"]["start_button"]["hover_text_color"]["a"].get<int>()),
        configJson["menu_ui"]["start_button"]["position"]["x"].get<float>(),
        configJson["menu_ui"]["start_button"]["position"]["y"].get<float>(),
        configJson["menu_ui"]["start_button"]["size"]["x"].get<float>(),
        configJson["menu_ui"]["start_button"]["size"]["y"].get<float>(),
        assetCont, "StartButton",
        [&ecsWorld, loadingBackgroundEntity, entity]()
        {
            // Get play scene entity
            riaecs::Entity playSceneEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::PLAY_SCENE);
            assert(playSceneEntity.IsValid() && "Play Scene entity is invalid!");

            // Get scene component
            mono_scene::ComponentScene* playScene = riaecs::GetComponentWithCheck<mono_scene::ComponentScene>(
                ecsWorld, playSceneEntity, mono_scene::ComponentSceneID(), "Play Scene", RIAECS_LOG_LOC);

            // Request load scene
            playScene->NeedsLoadRW()() = true;
            playScene->NeedsEditSystemListRW()() = true;

            // Get loading background's identity component
            mono_identity::ComponentIdentity* loadingBgIdentity = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, loadingBackgroundEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Show loading background
            loadingBgIdentity->SetActiveSelf(true);

            // Get start button's identity component
            mono_identity::ComponentIdentity* startButtonIdentity = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Hide start button
            startButtonIdentity->SetActiveSelf(false);
        });
    uiComponent->Setup(uiComponentParam);

    return entity;
}

riaecs::Entity bdc::menu_scene::CreateLoadingBackground
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const nlohmann::json& configJson
){
    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = menuSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "LoadingBackground";
    identityParam.activeSelf = false; // Initially hidden
    identity->Setup(identityParam);

    // UI
    ecsWorld.AddComponent(entity, mono_render::UIComponentID());
    mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
        ecsWorld, entity, mono_render::UIComponentID());
    mono_render::UIComponent::SetupParam uiParam;
    uiParam.ui_drawer = std::make_unique<bdc_ui::ImageUIDrawer>(
        window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
        1920.0f / 2.0f, 1080.0f / 2.0f,
        1920.0f, 1080.0f,
        LoadingBackgroundTextureAssetSourceID(), assetCont, "LoadingBackground");
    uiComponent->Setup(uiParam);
    
    return entity;
}

riaecs::Entity bdc::play_scene::CreateMainCamera
(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const riaecs::Entity &playerEntity, const nlohmann::json& configJson
){
    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = playSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "MainCamera";
    identity->Setup(identityParam);

    // Transform
    ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
        ecsWorld, entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform::SetupParam transformParam;
    transform->Setup(transformParam);
    transform->SetParent(entity, playerEntity, ecsWorld);
    transform->SetLocalPos(XMFLOAT3(
        configJson["camera"]["local_position"]["x"] * -1,
        configJson["camera"]["local_position"]["y"],
        configJson["camera"]["local_position"]["z"]), ecsWorld);
    transform->SetLocalRotFromEuler(
        configJson["camera"]["local_rotation"]["x"],
        configJson["camera"]["local_rotation"]["y"],
        configJson["camera"]["local_rotation"]["z"], ecsWorld);

    // Camera
    ecsWorld.AddComponent(entity, mono_render::ComponentCameraID());
    mono_render::ComponentCamera* camera = riaecs::GetComponent<mono_render::ComponentCamera>(
        ecsWorld, entity, mono_render::ComponentCameraID());
    mono_render::ComponentCamera::SetupParam cameraParam;
    cameraParam.fov_y = 90.0f;
    cameraParam.nearZ = 0.1f;
    cameraParam.farZ = 1000.0f;
    cameraParam.aspect_ratio = 16.0f / 9.0f;
    camera->Setup(cameraParam);

    return entity;
}

void bdc::play_scene::CreateLights
(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
    const nlohmann::json& configJson
){
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "DirectionalLight";
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pitch_ = XMConvertToRadians(80.0f);
        transformParam.roll_ = XMConvertToRadians(90.0f);
        transform->Setup(transformParam);

        // Directional Light
        ecsWorld.AddComponent(entity, mono_render::DirectionalLightComponentID());
        mono_render::DirectionalLightComponent* directionalLight = riaecs::GetComponent<mono_render::DirectionalLightComponent>(
            ecsWorld, entity, mono_render::DirectionalLightComponentID());
        mono_render::DirectionalLightComponent::SetupParam lightParam;
        lightParam.ortho_width = 1000.0f;
        lightParam.ortho_height = 1000.0f;
        lightParam.shadow_map_size = 4096;
        directionalLight->Setup(lightParam);
    }

    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
    
        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "AmbientLight";
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transform->Setup(transformParam);

        // Ambient Light
        ecsWorld.AddComponent(entity, mono_render::AmbientLightComponentID());
        mono_render::AmbientLightComponent* ambientLight = riaecs::GetComponent<mono_render::AmbientLightComponent>(
            ecsWorld, entity, mono_render::AmbientLightComponentID());
        mono_render::AmbientLightComponent::SetupParam lightParam;
        ambientLight->Setup(lightParam);
    }
}

riaecs::Entity bdc::play_scene::CreatePlayer
(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
){
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

    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = playSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "Player";
    identity->Setup(identityParam);

    // Transform
    ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
        ecsWorld, entity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform::SetupParam transformParam;
    {
        // Get the locator asset
        riaecs::ROObject<riaecs::IAsset> locatorAsset 
            = assetCont.Get({bdc::PlayerLocatorAssetSourceID(), assetCont.GetGeneration(bdc::PlayerLocatorAssetSourceID())});
        const mono_asset::AssetLocator* locAsset 
            = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());

        // Set the initial position from the asset
        const mono_asset::AssetLocator::LocatedTransform& firstTransform = locAsset->GetTransforms().front();
        transformParam.pos_ = firstTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(firstTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(firstTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(firstTransform.rotation_.z);
        transformParam.scale_ = firstTransform.scale_;
    }
    transform->Setup(transformParam);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get player albedo texture asset
    riaecs::ROObject<riaecs::IAsset> player_albedo_texture_asset 
        = assetCont.Get({bdc::PlayerAlbedoTextureAssetSourceID(), assetCont.GetGeneration(bdc::PlayerAlbedoTextureAssetSourceID())});
    const mono_asset::AssetTexture* player_albedo_texture_asset_ptr 
        = dynamic_cast<const mono_asset::AssetTexture*>(&player_albedo_texture_asset());

    // Get player enhanced albedo texture asset
    riaecs::ROObject<riaecs::IAsset> enhanced_player_albedo_texture_asset 
        = assetCont.Get({bdc::EnhancedPlayerAlbedoTextureAssetSourceID(), assetCont.GetGeneration(bdc::EnhancedPlayerAlbedoTextureAssetSourceID())});
    const mono_asset::AssetTexture* enhanced_player_albedo_texture_asset_ptr 
        = dynamic_cast<const mono_asset::AssetTexture*>(&enhanced_player_albedo_texture_asset());

    {
        // Create Lambert material setup param
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        lambert_material_param->albedo_texture_handle = player_albedo_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::PlayerLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(PlayerLambertMaterialHandleID().ID()),
            std::move(lambert_material_param));
    }

    {
        // Create Lambert material setup param for enhanced state
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        lambert_material_param->albedo_texture_handle = enhanced_player_albedo_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::EnhancedPlayerLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(EnhancedPlayerLambertMaterialHandleID().ID()),
            std::move(lambert_material_param));
    }

    // Get player model asset
    riaecs::ROObject<riaecs::IAsset> player_model_asset 
        = assetCont.Get({bdc::PlayerModelAssetSourceID(), assetCont.GetGeneration(bdc::PlayerModelAssetSourceID())});
    const mono_asset::AssetModel* player_model_asset_ptr 
        = dynamic_cast<const mono_asset::AssetModel*>(&player_model_asset());

    // MeshRenderer
    ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
    mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
        ecsWorld, entity, mono_render::ComponentMeshRendererID());
    mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
    meshRendererParam.vertex_buffer_handles = *player_model_asset_ptr->GetVertexBufferHandles();
    meshRendererParam.index_buffer_handles = *player_model_asset_ptr->GetIndexBufferHandles();
    meshRendererParam.index_counts = *player_model_asset_ptr->GetIndexCounts();

    std::vector<const render_graph::MaterialHandle*> material_handles;
    material_handles.resize(player_model_asset_ptr->GetIndexCounts()->size());
    for (int i = 0; i < material_handles.size(); ++i)
        material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::PlayerLambertMaterialHandleID().ID());
    meshRendererParam.material_handles = std::move(material_handles);
    meshRendererParam.cast_shadow = false;
    meshRenderer->Setup(meshRendererParam);

    // ObjectController
    ecsWorld.AddComponent(entity, mono_object_controller::ComponentObjectControllerID());
    mono_object_controller::ComponentObjectController* objectController
    = riaecs::GetComponent<mono_object_controller::ComponentObjectController>(
        ecsWorld, entity, mono_object_controller::ComponentObjectControllerID());
    mono_object_controller::ComponentObjectController::SetupParam objectControllerParam;
    objectControllerParam.handlerWindowEntity 
        = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    objectController->Setup(objectControllerParam);

    // BoxCollider
    ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
    mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
        ecsWorld, entity, mono_physics::ComponentBoxColliderID());
    mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
    boxColliderParam.isTrigger = false;
    {
        // Get the bounding box asset
        riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
            = assetCont.Get({bdc::PlayerBoundingBoxAssetSourceID(), assetCont.GetGeneration(bdc::PlayerBoundingBoxAssetSourceID())});
        const mono_asset::AssetBoundingBox* boxAsset 
            = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());

        // Set the box shape from the asset
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        boxColliderParam.box->SetMin(boxAsset->GetMin());
        boxColliderParam.box->SetMax(boxAsset->GetMax());
    }
    boxCollider->Setup(boxColliderParam);
    boxCollider->AddCollidableComponentID(mono_physics::ComponentBoxColliderID()); // Can collide with other box colliders

    // RigidBody
    ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
    mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
        ecsWorld, entity, mono_physics::ComponentRigidBodyID());
    mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
    rigidBodyParam.isStatic = false;
    rigidBody->Setup(rigidBodyParam);
    rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

    // Player
    ecsWorld.AddComponent(entity, bdc_player::ComponentPlayerID());
    bdc_player::ComponentPlayer* player = riaecs::GetComponent<bdc_player::ComponentPlayer>(
        ecsWorld, entity, bdc_player::ComponentPlayerID());
    bdc_player::ComponentPlayer::SetupParam playerParam;
    playerParam.handlerWindowEntity 
        = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    playerParam.moveSpeed = configJson["player"]["move_speed"];
    playerParam.enhancedMoveSpeed = configJson["player"]["enhanced_move_speed"];
    playerParam.normalMaterialHandleID = bdc::PlayerLambertMaterialHandleID().ID();
    playerParam.enhancedMaterialHandleID = bdc::EnhancedPlayerLambertMaterialHandleID().ID();
    player->Setup(playerParam);

    // Player starts in normal state
    ecsWorld.AddComponent(entity, bdc_player::ComponentPlayerNormalStateID());
    bdc_player::ComponentPlayerNormalState* normalState = riaecs::GetComponent<bdc_player::ComponentPlayerNormalState>(
        ecsWorld, entity, bdc_player::ComponentPlayerNormalStateID());
    bdc_player::ComponentPlayerNormalState::SetupParam normalStateParam;
    normalState->Setup(normalStateParam);

    // Submit command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

    return entity;
}

void bdc::play_scene::CreateMapElement(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, 
    const std::string &namePrefix, 
    size_t modelAssetSourceID, size_t albedoTexAssetSourceID, 
    size_t locatorAssetSourceID, size_t boundingBoxAssetSourceID, const nlohmann::json& configJson,
    size_t materialHandleID)
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

    // Get the locator asset
    riaecs::ROObject<riaecs::IAsset> locatorAsset 
        = assetCont.Get({locatorAssetSourceID, assetCont.GetGeneration(locatorAssetSourceID)});
    const mono_asset::AssetLocator* locAsset = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());
    assert(locAsset != nullptr);

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({boundingBoxAssetSourceID, assetCont.GetGeneration(boundingBoxAssetSourceID)});
    const mono_asset::AssetBoundingBox* boxAsset = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get albedo texture asset
    riaecs::ROObject<riaecs::IAsset> albedo_texture_asset 
        = assetCont.Get({albedoTexAssetSourceID, assetCont.GetGeneration(albedoTexAssetSourceID)});
    const mono_asset::AssetTexture* albedo_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&albedo_texture_asset());
    assert(albedo_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> model_asset 
        = assetCont.Get({modelAssetSourceID, assetCont.GetGeneration(modelAssetSourceID)});
    const mono_asset::AssetModel* model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&model_asset());
    assert(model_asset_ptr != nullptr);

    {
        // Create material for this map element
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = albedo_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(materialHandleID);

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(materialHandleID),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& locatedTransform : locAsset->GetTransforms())
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "map_" + namePrefix + "_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = locatedTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(locatedTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(locatedTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(locatedTransform.rotation_.z);
        transformParam.scale_ = locatedTransform.scale_;
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(materialHandleID);
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = false;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        {
            // Create 8 corners of the bounding box
            XMFLOAT3 corners[8] =
            {
                { boxAsset->GetMin().x, boxAsset->GetMin().y, boxAsset->GetMin().z },
                { boxAsset->GetMax().x, boxAsset->GetMin().y, boxAsset->GetMin().z },
                { boxAsset->GetMin().x, boxAsset->GetMax().y, boxAsset->GetMin().z },
                { boxAsset->GetMax().x, boxAsset->GetMax().y, boxAsset->GetMin().z },
                { boxAsset->GetMin().x, boxAsset->GetMin().y, boxAsset->GetMax().z },
                { boxAsset->GetMax().x, boxAsset->GetMin().y, boxAsset->GetMax().z },
                { boxAsset->GetMin().x, boxAsset->GetMax().y, boxAsset->GetMax().z },
                { boxAsset->GetMax().x, boxAsset->GetMax().y, boxAsset->GetMax().z },
            };

            // Rotate 8 corners
            XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(
                XMConvertToRadians(locatedTransform.rotation_.x),
                XMConvertToRadians(locatedTransform.rotation_.y),
                XMConvertToRadians(locatedTransform.rotation_.z));

            for (XMFLOAT3& corner : corners)
            {
                XMVECTOR cornerVec = XMLoadFloat3(&corner);
                cornerVec = XMVector3Rotate(cornerVec, rotationQuat);
                XMStoreFloat3(&corner, cornerVec);
            }

            // Find the new min and max from the rotated corners
            XMFLOAT3 newMin = corners[0];
            XMFLOAT3 newMax = corners[0];
            for (const XMFLOAT3& corner : corners)
            {
                newMin.x = std::min(newMin.x, corner.x);
                newMin.y = std::min(newMin.y, corner.y);
                newMin.z = std::min(newMin.z, corner.z);

                newMax.x = std::max(newMax.x, corner.x);
                newMax.y = std::max(newMax.y, corner.y);
                newMax.z = std::max(newMax.z, corner.z);
            }

            // Set min and max
            boxColliderParam.box->SetMin(newMin);
            boxColliderParam.box->SetMax(newMax);
        }
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = true;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());
    }

    // Submit command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateMapElement(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const std::string &namePrefix, 
    size_t modelAssetSourceID, size_t locatorAssetSourceID, size_t boundingBoxAssetSourceID, 
    const nlohmann::json& configJson, size_t materialHandleID)
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

    // Get the locator asset
    riaecs::ROObject<riaecs::IAsset> locatorAsset 
        = assetCont.Get({locatorAssetSourceID, assetCont.GetGeneration(locatorAssetSourceID)});
    const mono_asset::AssetLocator* locAsset = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());
    assert(locAsset != nullptr);

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({boundingBoxAssetSourceID, assetCont.GetGeneration(boundingBoxAssetSourceID)});
    const mono_asset::AssetBoundingBox* boxAsset = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> model_asset 
        = assetCont.Get({modelAssetSourceID, assetCont.GetGeneration(modelAssetSourceID)});
    const mono_asset::AssetModel* model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&model_asset());
    assert(model_asset_ptr != nullptr);

    {
        // Create material for this map element
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(materialHandleID);

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(materialHandleID),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& locatedTransform : locAsset->GetTransforms())
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "map_" + namePrefix + "_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = locatedTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(locatedTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(locatedTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(locatedTransform.rotation_.z);
        transformParam.scale_ = locatedTransform.scale_;
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(materialHandleID);
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = false;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        {
            // Create 8 corners of the bounding box
            XMFLOAT3 corners[8] =
            {
                { boxAsset->GetMin().x, boxAsset->GetMin().y, boxAsset->GetMin().z },
                { boxAsset->GetMax().x, boxAsset->GetMin().y, boxAsset->GetMin().z },
                { boxAsset->GetMin().x, boxAsset->GetMax().y, boxAsset->GetMin().z },
                { boxAsset->GetMax().x, boxAsset->GetMax().y, boxAsset->GetMin().z },
                { boxAsset->GetMin().x, boxAsset->GetMin().y, boxAsset->GetMax().z },
                { boxAsset->GetMax().x, boxAsset->GetMin().y, boxAsset->GetMax().z },
                { boxAsset->GetMin().x, boxAsset->GetMax().y, boxAsset->GetMax().z },
                { boxAsset->GetMax().x, boxAsset->GetMax().y, boxAsset->GetMax().z },
            };

            // Rotate 8 corners
            XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(
                XMConvertToRadians(locatedTransform.rotation_.x),
                XMConvertToRadians(locatedTransform.rotation_.y),
                XMConvertToRadians(locatedTransform.rotation_.z));

            for (XMFLOAT3& corner : corners)
            {
                XMVECTOR cornerVec = XMLoadFloat3(&corner);
                cornerVec = XMVector3Rotate(cornerVec, rotationQuat);
                XMStoreFloat3(&corner, cornerVec);
            }

            // Find the new min and max from the rotated corners
            XMFLOAT3 newMin = corners[0];
            XMFLOAT3 newMax = corners[0];
            for (const XMFLOAT3& corner : corners)
            {
                newMin.x = std::min(newMin.x, corner.x);
                newMin.y = std::min(newMin.y, corner.y);
                newMin.z = std::min(newMin.z, corner.z);

                newMax.x = std::max(newMax.x, corner.x);
                newMax.y = std::max(newMax.y, corner.y);
                newMax.z = std::max(newMax.z, corner.z);
            }

            // Set min and max
            boxColliderParam.box->SetMin(newMin);
            boxColliderParam.box->SetMax(newMax);
        }
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = true;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());
    }

    // Submit command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateMapElement(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, const riaecs::Entity &playSceneEntity, 
    riaecs::StagingEntityArea &stagingArea, const std::string &namePrefix, 
    size_t modelAssetSourceID, size_t albedoTexAssetSourceID, size_t locatorAssetSourceID, size_t materialHandleID)
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

    // Get the locator asset
    riaecs::ROObject<riaecs::IAsset> locatorAsset 
        = assetCont.Get({locatorAssetSourceID, assetCont.GetGeneration(locatorAssetSourceID)});
    const mono_asset::AssetLocator* locAsset = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());
    assert(locAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get albedo texture asset
    riaecs::ROObject<riaecs::IAsset> albedo_texture_asset 
        = assetCont.Get({albedoTexAssetSourceID, assetCont.GetGeneration(albedoTexAssetSourceID)});
    const mono_asset::AssetTexture* albedo_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&albedo_texture_asset());
    assert(albedo_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> model_asset 
        = assetCont.Get({modelAssetSourceID, assetCont.GetGeneration(modelAssetSourceID)});
    const mono_asset::AssetModel* model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&model_asset());
    assert(model_asset_ptr != nullptr);

    {
        // Create material for this map element
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = albedo_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(materialHandleID);

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(materialHandleID),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& locatedTransform : locAsset->GetTransforms())
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "map_" + namePrefix + "_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = locatedTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(locatedTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(locatedTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(locatedTransform.rotation_.z);
        transformParam.scale_ = locatedTransform.scale_;
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(materialHandleID);
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);
    }

    // Submit command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateMap(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson)
{
    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Floor",
        bdc::FloorModelAssetSourceID(), bdc::FloorAlbedoTextureAssetSourceID(), bdc::FloorLocatorAssetSourceID(),
        bdc::FloorLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "BookShelf",
        bdc::BookShelfModelAssetSourceID(), bdc::BookShelfAlbedoTextureAssetSourceID(),
        bdc::BookShelfLocatorAssetSourceID(), bdc::BookShelfBoundingBoxAssetSourceID(), configJson,
        bdc::BookShelfLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "CatScratch",
        bdc::CatScratchModelAssetSourceID(), bdc::CatScratchAlbedoTextureAssetSourceID(),
        bdc::CatScratchLocatorAssetSourceID(), bdc::CatScratchBoundingBoxAssetSourceID(), configJson,
        bdc::CatScratchLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "FoodShelf01",
        bdc::FoodShelf01ModelAssetSourceID(), bdc::FoodShelf01AlbedoTextureAssetSourceID(),
        bdc::FoodShelf01LocatorAssetSourceID(), bdc::FoodShelf01BoundingBoxAssetSourceID(), configJson,
        bdc::FoodShelf01LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "FoodShelf02",
        bdc::FoodShelf02ModelAssetSourceID(), bdc::FoodShelf02AlbedoTextureAssetSourceID(),
        bdc::FoodShelf02LocatorAssetSourceID(), bdc::FoodShelf02BoundingBoxAssetSourceID(), configJson,
        bdc::FoodShelf02LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Table4x20",
        bdc::Table4x20ModelAssetSourceID(), bdc::Table4x20AlbedoTextureAssetSourceID(),
        bdc::Table4x20LocatorAssetSourceID(), bdc::Table4x20BoundingBoxAssetSourceID(), configJson,
        bdc::Table4x20LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Table4x6",
        bdc::Table4x6ModelAssetSourceID(), bdc::Table4x6AlbedoTextureAssetSourceID(),
        bdc::Table4x6LocatorAssetSourceID(), bdc::Table4x6BoundingBoxAssetSourceID(), configJson,
        bdc::Table4x6LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Table6x8",
        bdc::Table6x8ModelAssetSourceID(), bdc::Table6x8AlbedoTextureAssetSourceID(),
        bdc::Table6x8LocatorAssetSourceID(), bdc::Table6x8BoundingBoxAssetSourceID(), configJson,
        bdc::Table6x8LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Table4x28",
        bdc::Table4x28ModelAssetSourceID(), bdc::Table4x28AlbedoTextureAssetSourceID(),
        bdc::Table4x28LocatorAssetSourceID(), bdc::Table4x28BoundingBoxAssetSourceID(), configJson,
        bdc::Table4x28LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Wall1",
        bdc::Wall1ModelAssetSourceID(), bdc::Wall1AlbedoTextureAssetSourceID(),
        bdc::Wall1LocatorAssetSourceID(), bdc::Wall1BoundingBoxAssetSourceID(), configJson,
        bdc::Wall1LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Wall2",
        bdc::Wall2ModelAssetSourceID(), bdc::Wall2AlbedoTextureAssetSourceID(),
        bdc::Wall2LocatorAssetSourceID(), bdc::Wall2BoundingBoxAssetSourceID(), configJson,
        bdc::Wall2LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Refrigerator",
        bdc::RefrigeratorModelAssetSourceID(),
        bdc::RefrigeratorLocatorAssetSourceID(), bdc::RefrigeratorBoundingBoxAssetSourceID(), configJson,
        bdc::RefrigeratorLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Kitchen",
        bdc::KitchenModelAssetSourceID(), bdc::KitchenAlbedoTextureAssetSourceID(),
        bdc::KitchenLocatorAssetSourceID(), bdc::KitchenBoundingBoxAssetSourceID(), configJson,
        bdc::KitchenLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Chair2x2",
        bdc::Chair2x2ModelAssetSourceID(), bdc::Chair2x2AlbedoTextureAssetSourceID(),
        bdc::Chair2x2LocatorAssetSourceID(), bdc::Chair2x2BoundingBoxAssetSourceID(), configJson,
        bdc::Chair2x2LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Chair2x4",
        bdc::Chair2x4ModelAssetSourceID(), bdc::Chair2x4AlbedoTextureAssetSourceID(),
        bdc::Chair2x4LocatorAssetSourceID(), bdc::Chair2x4BoundingBoxAssetSourceID(), configJson,
        bdc::Chair2x4LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "Sofa",
        bdc::SofaModelAssetSourceID(), bdc::SofaAlbedoTextureAssetSourceID(),
        bdc::SofaLocatorAssetSourceID(), bdc::SofaBoundingBoxAssetSourceID(), configJson,
        bdc::SofaLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "MouseTrap",
        bdc::MouseTrapModelAssetSourceID(), bdc::MouseTrapAlbedoTextureAssetSourceID(),
        bdc::MouseTrapLocatorAssetSourceID(), bdc::MouseTrapBoundingBoxAssetSourceID(), configJson,
        bdc::MouseTrapLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "SofaTable",
        bdc::SofaTableModelAssetSourceID(), bdc::SofaTableAlbedoTextureAssetSourceID(),
        bdc::SofaTableLocatorAssetSourceID(), bdc::SofaTableBoundingBoxAssetSourceID(), configJson,
        bdc::SofaTableLambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox01",
        bdc::WoodBox01ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox01LocatorAssetSourceID(), bdc::WoodBox01BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox01LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox02",
        bdc::WoodBox02ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox02LocatorAssetSourceID(), bdc::WoodBox02BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox02LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox03",
        bdc::WoodBox03ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox03LocatorAssetSourceID(), bdc::WoodBox03BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox03LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox04",
        bdc::WoodBox04ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox04LocatorAssetSourceID(), bdc::WoodBox04BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox04LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox05",
        bdc::WoodBox05ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox05LocatorAssetSourceID(), bdc::WoodBox05BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox05LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox06",
        bdc::WoodBox06ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox06LocatorAssetSourceID(), bdc::WoodBox06BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox06LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox07",
        bdc::WoodBox07ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox07LocatorAssetSourceID(), bdc::WoodBox07BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox07LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox08",
        bdc::WoodBox08ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox08LocatorAssetSourceID(), bdc::WoodBox08BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox08LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox09",
        bdc::WoodBox09ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox09LocatorAssetSourceID(), bdc::WoodBox09BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox09LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox10",
        bdc::WoodBox10ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox10LocatorAssetSourceID(), bdc::WoodBox10BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox10LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox11",
        bdc::WoodBox11ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox11LocatorAssetSourceID(), bdc::WoodBox11BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox11LambertMaterialHandleID().ID());

    play_scene::CreateMapElement(
        ecsWorld, assetCont, playSceneEntity, stagingArea,
        "WoodBox12",
        bdc::WoodBox12ModelAssetSourceID(), bdc::WoodBoxAlbedoTextureAssetSourceID(),
        bdc::WoodBox12LocatorAssetSourceID(), bdc::WoodBox12BoundingBoxAssetSourceID(), configJson,
        bdc::WoodBox12LambertMaterialHandleID().ID());
}

void bdc::play_scene::CreateDots(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson)
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

    // Get the locator asset
    riaecs::ROObject<riaecs::IAsset> locatorAsset 
        = assetCont.Get({bdc::DotLocatorAssetSourceID(), assetCont.GetGeneration(bdc::DotLocatorAssetSourceID())});
    const mono_asset::AssetLocator* locAsset = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());
    assert(locAsset != nullptr);

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({bdc::DotBoundingBoxAssetSourceID(), assetCont.GetGeneration(bdc::DotBoundingBoxAssetSourceID())});
    const mono_asset::AssetBoundingBox* boxAsset = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get dot model asset
    riaecs::ROObject<riaecs::IAsset> dot_model_asset 
        = assetCont.Get({bdc::DotModelAssetSourceID(), assetCont.GetGeneration(bdc::DotModelAssetSourceID())});
    const mono_asset::AssetModel* dot_model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&dot_model_asset());
    assert(dot_model_asset_ptr != nullptr);

    {
        // Create material for dot
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::DotLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(bdc::DotLambertMaterialHandleID().ID()),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& locatedTransform : locAsset->GetTransforms())
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "Dot_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = locatedTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(locatedTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(locatedTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(locatedTransform.rotation_.z);
        transformParam.scale_ = locatedTransform.scale_;
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *dot_model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *dot_model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *dot_model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(dot_model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::DotLambertMaterialHandleID().ID());
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = true;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        boxColliderParam.box->SetMin(boxAsset->GetMin());
        boxColliderParam.box->SetMax(boxAsset->GetMax());
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = true;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

        // Dot
        ecsWorld.AddComponent(entity, bdc_dot::ComponentDotID());
        bdc_dot::ComponentDot* dot = riaecs::GetComponent<bdc_dot::ComponentDot>(
            ecsWorld, entity, bdc_dot::ComponentDotID());
        bdc_dot::ComponentDot::SetupParam dotParam;
        dot->Setup(dotParam);

        createdEntityCount++;
    }

    // Submit graphics command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateLargeEnemies(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson)
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

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({bdc::LargeEnemyBoundingBoxAssetSourceID(), assetCont.GetGeneration(bdc::LargeEnemyBoundingBoxAssetSourceID())});
    const mono_asset::AssetBoundingBox* boxAsset = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> large_enemy_model_asset 
        = assetCont.Get({bdc::LargeEnemyModelAssetSourceID(), assetCont.GetGeneration(bdc::LargeEnemyModelAssetSourceID())});
    const mono_asset::AssetModel* large_enemy_model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&large_enemy_model_asset());
    assert(large_enemy_model_asset_ptr != nullptr);

    {
        // Create material for large enemy
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::LargeEnemyLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(bdc::LargeEnemyLambertMaterialHandleID().ID()),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& jsonEnemies : configJson["large_enemies"])
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "LargeEnemy_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = XMFLOAT3(
            jsonEnemies["spawn_point"]["x"] * -1, 
            jsonEnemies["spawn_point"]["y"], 
            jsonEnemies["spawn_point"]["z"]);
        transformParam.scale_ = XMFLOAT3(
            configJson["large_enemy"]["scale"]["x"], 
            configJson["large_enemy"]["scale"]["y"], 
            configJson["large_enemy"]["scale"]["z"]);
        
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *large_enemy_model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *large_enemy_model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *large_enemy_model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(large_enemy_model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::LargeEnemyLambertMaterialHandleID().ID());
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = true;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        boxColliderParam.box->SetMin(boxAsset->GetMin());
        boxColliderParam.box->SetMax(boxAsset->GetMax());
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = false;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

        // Enemy
        ecsWorld.AddComponent(entity, bdc_enemy::ComponentEnemyID());
        bdc_enemy::ComponentEnemy* enemy = riaecs::GetComponent<bdc_enemy::ComponentEnemy>(
            ecsWorld, entity, bdc_enemy::ComponentEnemyID());
        bdc_enemy::ComponentEnemy::SetupParam enemyParam;
        enemyParam.moveSpeed = configJson["large_enemy"]["move_speed"];
        enemyParam.goingWayPointIndex = 0;
        
        for (const auto& jsonWaypoint : jsonEnemies["waypoints"])
            enemyParam.wayPoints.emplace_back(
                XMFLOAT3(jsonWaypoint["x"] * -1, jsonWaypoint["y"], jsonWaypoint["z"]));

        enemy->Setup(enemyParam);
        createdEntityCount++;
    }

    // Submit graphics command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateSmallEnemies(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson)
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

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({bdc::SmallEnemyBoundingBoxAssetSourceID(), assetCont.GetGeneration(bdc::SmallEnemyBoundingBoxAssetSourceID())});
    const mono_asset::AssetBoundingBox* boxAsset = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> small_enemy_model_asset 
        = assetCont.Get({bdc::SmallEnemyModelAssetSourceID(), assetCont.GetGeneration(bdc::SmallEnemyModelAssetSourceID())});
    const mono_asset::AssetModel* small_enemy_model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&small_enemy_model_asset());
    assert(small_enemy_model_asset_ptr != nullptr);

    {
        // Create material for small enemy
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::SmallEnemyLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(bdc::SmallEnemyLambertMaterialHandleID().ID()),
            std::move(material_param));
    }

    size_t createdEntityCount = 0;
    for (const auto& jsonEnemies : configJson["small_enemies"])
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "SmallEnemy_" + std::to_string(createdEntityCount);
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = XMFLOAT3(
            jsonEnemies["spawn_point"]["x"] * -1, 
            jsonEnemies["spawn_point"]["y"], 
            jsonEnemies["spawn_point"]["z"]);
        transformParam.scale_ = XMFLOAT3(
            configJson["small_enemy"]["scale"]["x"], 
            configJson["small_enemy"]["scale"]["y"], 
            configJson["small_enemy"]["scale"]["z"]);
        
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *small_enemy_model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *small_enemy_model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *small_enemy_model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(small_enemy_model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::SmallEnemyLambertMaterialHandleID().ID());
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = true;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        boxColliderParam.box->SetMin(boxAsset->GetMin());
        boxColliderParam.box->SetMax(boxAsset->GetMax());
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = false;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

        // Enemy
        ecsWorld.AddComponent(entity, bdc_enemy::ComponentEnemyID());
        bdc_enemy::ComponentEnemy* enemy = riaecs::GetComponent<bdc_enemy::ComponentEnemy>(
            ecsWorld, entity, bdc_enemy::ComponentEnemyID());
        bdc_enemy::ComponentEnemy::SetupParam enemyParam;
        enemyParam.moveSpeed = configJson["small_enemy"]["move_speed"];
        enemyParam.goingWayPointIndex = 0;

        for (const auto& jsonWaypoint : jsonEnemies["waypoints"])
            enemyParam.wayPoints.emplace_back(
                XMFLOAT3(jsonWaypoint["x"] * -1, jsonWaypoint["y"], jsonWaypoint["z"]));

        enemy->Setup(enemyParam);
        createdEntityCount++;
    }

    // Submit graphics command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
}

void bdc::play_scene::CreateGameFlow(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, 
    const nlohmann::json &configJson, 
    const riaecs::Entity &playerEntity, const riaecs::Entity &cageEntity,
    const std::vector<riaecs::Entity> &gameOverUIEntities,
    const std::vector<riaecs::Entity> &crystalEntities)
{
    riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

    // SceneTag
    ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, entity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = playSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, entity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "GameFlowController";
    identity->Setup(identityParam);

    // GameFlow
    ecsWorld.AddComponent(entity, bdc_game_flow::ComponentGameFlowID());
    bdc_game_flow::ComponentGameFlow* gameFlow = riaecs::GetComponent<bdc_game_flow::ComponentGameFlow>(
        ecsWorld, entity, bdc_game_flow::ComponentGameFlowID());
    bdc_game_flow::ComponentGameFlow::SetupParam gameFlowParam;
    gameFlowParam.needDotCountToSpawnCrystal = configJson["game_flow"]["need_dot_count_to_spawn_crystal"];
    gameFlowParam.needCrystalCountToClearPhase = configJson["game_flow"]["need_crystal_count_to_clear_phase"];
    gameFlowParam.needDeadCountToGameOver = configJson["game_flow"]["need_dead_count_to_game_over"];
    gameFlow->Setup(gameFlowParam);

    bdc_game_flow::GameStateMachine& stateMachine = gameFlow->GetStateMachine();

    /*******************************************************************************************************************
     * State transition from null state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        mono_cycle::STATE_NULL, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::START, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::START,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for START event
        riaecs::Log::OutToConsole("Game Flow Started. Entering START state.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    /*******************************************************************************************************************
     * State transition from START state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::START, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_GET, 
        mono_cycle::STATE_NULL,
        [playerEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_GET event
        riaecs::Log::OutToConsole("Dot gotten in START state.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot got flag in player component
        player->SetIsGotDot(false);

        return mono_cycle::NextStateType::STAY;
    });

    // Get phase 1 cage move speed from config
    float phase1CageMoveSpeed = configJson["cage"]["phase_1_move_speed"].get<float>();

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::START, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_COLLECT_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1,
        [playerEntity, crystalEntities, cageEntity, phase1CageMoveSpeed](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_COLLECT_COUNT_REACHED event
        riaecs::Log::OutToConsole("Dot collect count reached in START state. Entering PHASE_1.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot collected count in player component
        player->SetDotCollectCount(0);

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        // Collect unspawned crystal indices
        std::vector<size_t> unspawnedCrystalIndices;
        for (size_t i = 0; i < crystalEntities.size(); ++i)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[i], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntities[i], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is already spawned
            if (!crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                unspawnedCrystalIndices.push_back(i);
        }

        DirectX::XMFLOAT3 spawnedCrystalPos = {};
        if (!unspawnedCrystalIndices.empty())
        {
            // Select a random index from unspawned crystals
            std::uniform_int_distribution<size_t> dist(0, unspawnedCrystalIndices.size() - 1);
            size_t crystalEntityIndex = unspawnedCrystalIndices[dist(mt)];

            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[crystalEntityIndex], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Activate the crystal entity
            crystalIdentity->SetActiveSelf(true);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntities[crystalEntityIndex], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Get transform component
            mono_transform::ComponentTransform* crystalTransform
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                ecsWorld, crystalEntities[crystalEntityIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

            spawnedCrystalPos = crystalTransform->GetPos();
        }

        // Get cage component
        bdc_cage::ComponentCage* cage
        = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
            ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

        // Set cage target position to the spawned crystal position
        cage->SetTargetPos(spawnedCrystalPos);
        cage->ResetCellTimer();

        // Set cage move speed for phase 1
        cage->SetMoveSpeed(phase1CageMoveSpeed);
        
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::START, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::RE_SPAWN,
        [entity, gameOverUIEntities](riaecs::IECSWorld& ecsWorld)
    {
        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlowController", RIAECS_LOG_LOC);

        // Set system loop command
        gameFlow->SetCurrentSystemLoopCommand(std::make_unique<PlaySceneWhenDeadSystemLoopCommand>());

        for (const riaecs::Entity& uiEntity : gameOverUIEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* gameOverUIIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, uiEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Show game over UI
            gameOverUIIdentity->SetActiveSelf(true);
        }

        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::START);

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::START, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_OVER,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PLAYER_DEAD_COUNT_REACHED event
        riaecs::Log::OutToConsole("Player dead count reached in START state. Game Over.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::START, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PAUSE, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::MENU,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PAUSE event
        riaecs::Log::OutToConsole("Game paused from START state. Entering MENU.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::START);

    /*******************************************************************************************************************
     * State transition from PHASE_1 state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_GET, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1,
        [playerEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_GET event
        riaecs::Log::OutToConsole("Dot gotten in PHASE_1 state.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot got flag in player component
        player->SetIsGotDot(false);
        
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_COLLECT_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2,
        [playerEntity, crystalEntities, cageEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_COLLECT_COUNT_REACHED event
        riaecs::Log::OutToConsole("Dot collect count reached in PHASE_1 state. Spawning crystal\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot collected count in player component
        player->SetDotCollectCount(0);

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        // Collect unspawned crystal indices
        std::vector<size_t> unspawnedCrystalIndices;
        for (size_t i = 0; i < crystalEntities.size(); ++i)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[i], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntities[i], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is already spawned
            if (!crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                unspawnedCrystalIndices.push_back(i);
        }

        if (!unspawnedCrystalIndices.empty())
        {
            // Select a random index from unspawned crystals
            std::uniform_int_distribution<size_t> dist(0, unspawnedCrystalIndices.size() - 1);
            size_t crystalEntityIndex = unspawnedCrystalIndices[dist(mt)];

            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[crystalEntityIndex], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Activate the crystal entity
            crystalIdentity->SetActiveSelf(true);
        }

        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_GET, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1,
        [playerEntity, crystalEntities, cageEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for CRYSTAL_GET event
        riaecs::Log::OutToConsole("Crystal gotten in PHASE_1 state.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset crystal got flag in player component
        player->SetIsGotCrystal(false);

        // Get spawned but not taken crystal entities
        std::vector<riaecs::Entity> spawnedCrystalEntities;
        for (const riaecs::Entity& crystalEntity : crystalEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntity, bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is spawned and not taken
            if (crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                spawnedCrystalEntities.push_back(crystalEntity);
        }

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        if (!spawnedCrystalEntities.empty())
        {
            // Select a random index from spawned crystals
            std::uniform_int_distribution<size_t> dist(0, spawnedCrystalEntities.size() - 1);
            size_t selectedIndex = dist(mt);

            // Get transform component
            mono_transform::ComponentTransform* crystalTransform
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                ecsWorld, spawnedCrystalEntities[selectedIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

            // Get cage component
            bdc_cage::ComponentCage* cage
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

            // Set cage target position to the gotten crystal position
            cage->SetTargetPos(crystalTransform->GetPos());
            cage->ResetCellTimer();
        }
        else
        {
            // Collect unspawned crystal indices
            std::vector<size_t> unspawnedCrystalIndices;
            for (size_t i = 0; i < crystalEntities.size(); ++i)
            {
                // Get identity component
                mono_identity::ComponentIdentity* crystalIdentity
                = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, crystalEntities[i], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Get crystal component
                bdc_interactive_object::ComponentCrystal* crystal
                = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                    ecsWorld, crystalEntities[i], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

                // Check if the crystal is already spawned
                if (!crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                    unspawnedCrystalIndices.push_back(i);
            }

            DirectX::XMFLOAT3 spawnedCrystalPos = {};
            if (!unspawnedCrystalIndices.empty())
            {
                // Select a random index from unspawned crystals
                std::uniform_int_distribution<size_t> dist(0, unspawnedCrystalIndices.size() - 1);
                size_t selectedIndex = unspawnedCrystalIndices[dist(mt)];
                size_t crystalEntityIndex = unspawnedCrystalIndices[selectedIndex];

                // Get identity component
                mono_identity::ComponentIdentity* crystalIdentity
                = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, crystalEntities[crystalEntityIndex], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Activate the crystal entity
                crystalIdentity->SetActiveSelf(true);

                // Get crystal component
                bdc_interactive_object::ComponentCrystal* crystal
                = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                    ecsWorld, crystalEntities[crystalEntityIndex], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

                // Get transform component
                mono_transform::ComponentTransform* crystalTransform
                = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                    ecsWorld, crystalEntities[crystalEntityIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

                spawnedCrystalPos = crystalTransform->GetPos();
            }

            // Get cage component
            bdc_cage::ComponentCage* cage
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

            // Set cage target position to the spawned crystal position
            cage->SetTargetPos(spawnedCrystalPos);
            cage->ResetCellTimer();
        }

        return mono_cycle::NextStateType::STAY;
    });

    // Get phase 2 cage move speed from config
    float phase2CageMoveSpeed = configJson["cage"]["phase_2_move_speed"].get<float>();

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_COLLECT_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1,
        [playerEntity, crystalEntities, cageEntity, phase2CageMoveSpeed](riaecs::IECSWorld& ecsWorld)
    {
        // Action for CRYSTAL_COLLECT_COUNT_REACHED event
        riaecs::Log::OutToConsole("Crystal collect count reached in PHASE_1 state. Entering PHASE_2.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset crystal collected count in player component
        player->SetCrystalCollectCount(0);

        // Get spawned but not taken crystal entities
        std::vector<riaecs::Entity> spawnedCrystalEntities;
        for (const riaecs::Entity& crystalEntity : crystalEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntity, bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is spawned and not taken
            if (crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                spawnedCrystalEntities.push_back(crystalEntity);
        }

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        if (!spawnedCrystalEntities.empty())
        {
            // Select a random index from spawned crystals
            std::uniform_int_distribution<size_t> dist(0, spawnedCrystalEntities.size() - 1);
            size_t selectedIndex = dist(mt);

            // Get transform component
            mono_transform::ComponentTransform* crystalTransform
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                ecsWorld, spawnedCrystalEntities[selectedIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

            // Get cage component
            bdc_cage::ComponentCage* cage
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

            // Set cage target position to the gotten crystal position
            cage->SetTargetPos(crystalTransform->GetPos());
            cage->ResetCellTimer();
        }

        // Get cage component
        bdc_cage::ComponentCage* cage
        = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
            ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

        // Set cage move speed for phase 2
        cage->SetMoveSpeed(phase2CageMoveSpeed);
        
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::RE_SPAWN,
        [entity, gameOverUIEntities](riaecs::IECSWorld& ecsWorld)
    {
        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlowController", RIAECS_LOG_LOC);

        // Set system loop command
        gameFlow->SetCurrentSystemLoopCommand(std::make_unique<PlaySceneWhenDeadSystemLoopCommand>());

        for (const riaecs::Entity& uiEntity : gameOverUIEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* gameOverUIIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, uiEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Show game over UI
            gameOverUIIdentity->SetActiveSelf(true);
        }
        
        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1);

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_OVER,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PLAYER_DEAD_COUNT_REACHED event
        riaecs::Log::OutToConsole("Player dead count reached in PHASE_1 state. Game Over.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PAUSE, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::MENU,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PAUSE event
        riaecs::Log::OutToConsole("Game paused from PHASE_1 state. Entering MENU.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1);

    /*******************************************************************************************************************
     * State transition from PHASE_2 state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_GET, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2,
        [playerEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_GET event
        riaecs::Log::OutToConsole("Dot gotten in PHASE_2 state.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot got flag in player component
        player->SetIsGotDot(false);
        
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_COLLECT_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2,
        [playerEntity, crystalEntities, cageEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for DOT_COLLECT_COUNT_REACHED event
        riaecs::Log::OutToConsole("Dot collect count reached in PHASE_2 state. Spawning crystal\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset dot collected count in player component
        player->SetDotCollectCount(0);

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        // Collect unspawned crystal indices
        std::vector<size_t> unspawnedCrystalIndices;
        for (size_t i = 0; i < crystalEntities.size(); ++i)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[i], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntities[i], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is already spawned
            if (!crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                unspawnedCrystalIndices.push_back(i);
        }

        if (!unspawnedCrystalIndices.empty())
        {
            // Select a random index from unspawned crystals
            std::uniform_int_distribution<size_t> dist(0, unspawnedCrystalIndices.size() - 1);
            size_t crystalEntityIndex = unspawnedCrystalIndices[dist(mt)];

            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntities[crystalEntityIndex], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Activate the crystal entity
            crystalIdentity->SetActiveSelf(true);
        }
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_GET, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2,
        [playerEntity, crystalEntities, cageEntity](riaecs::IECSWorld& ecsWorld)
    {
        // Action for CRYSTAL_GET event
        riaecs::Log::OutToConsole("Crystal gotten in PHASE_2 state.\n");

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, playerEntity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Reset crystal got flag in player component
        player->SetIsGotCrystal(false);

        // Get spawned but not taken crystal entities
        std::vector<riaecs::Entity> spawnedCrystalEntities;
        for (const riaecs::Entity& crystalEntity : crystalEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* crystalIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, crystalEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Get crystal component
            bdc_interactive_object::ComponentCrystal* crystal
            = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                ecsWorld, crystalEntity, bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

            // Check if the crystal is spawned and not taken
            if (crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                spawnedCrystalEntities.push_back(crystalEntity);
        }

        // Get random value generator
        std::random_device rd;
        std::mt19937 mt(rd());

        if (!spawnedCrystalEntities.empty())
        {
            // Select a random index from spawned crystals
            std::uniform_int_distribution<size_t> dist(0, spawnedCrystalEntities.size() - 1);
            size_t selectedIndex = dist(mt);

            // Get transform component
            mono_transform::ComponentTransform* crystalTransform
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                ecsWorld, spawnedCrystalEntities[selectedIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

            // Get cage component
            bdc_cage::ComponentCage* cage
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

            // Set cage target position to the gotten crystal position
            cage->SetTargetPos(crystalTransform->GetPos());
            cage->ResetCellTimer();
        }
        else
        {
            // Collect unspawned crystal indices
            std::vector<size_t> unspawnedCrystalIndices;
            for (size_t i = 0; i < crystalEntities.size(); ++i)
            {
                // Get identity component
                mono_identity::ComponentIdentity* crystalIdentity
                = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, crystalEntities[i], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Get crystal component
                bdc_interactive_object::ComponentCrystal* crystal
                = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                    ecsWorld, crystalEntities[i], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

                // Check if the crystal is already spawned
                if (!crystalIdentity->IsActiveSelf() && !crystal->IsTaken())
                    unspawnedCrystalIndices.push_back(i);
            }

            DirectX::XMFLOAT3 spawnedCrystalPos = {};
            if (!unspawnedCrystalIndices.empty())
            {
                // Select a random index from unspawned crystals
                std::uniform_int_distribution<size_t> dist(0, unspawnedCrystalIndices.size() - 1);
                size_t selectedIndex = unspawnedCrystalIndices[dist(mt)];
                size_t crystalEntityIndex = unspawnedCrystalIndices[selectedIndex];

                // Get identity component
                mono_identity::ComponentIdentity* crystalIdentity
                = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, crystalEntities[crystalEntityIndex], mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Activate the crystal entity
                crystalIdentity->SetActiveSelf(true);

                // Get crystal component
                bdc_interactive_object::ComponentCrystal* crystal
                = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                    ecsWorld, crystalEntities[crystalEntityIndex], bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

                // Get transform component
                mono_transform::ComponentTransform* crystalTransform
                = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                    ecsWorld, crystalEntities[crystalEntityIndex], mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

                spawnedCrystalPos = crystalTransform->GetPos();
            }

            // Get cage component
            bdc_cage::ComponentCage* cage
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, cageEntity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

            // Set cage target position to the spawned crystal position
            cage->SetTargetPos(spawnedCrystalPos);
            cage->ResetCellTimer();
        }
        
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_COLLECT_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_CLEAR,
        [playerEntity, entity](riaecs::IECSWorld& ecsWorld)
    {
        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlowController", RIAECS_LOG_LOC);

        // Set system loop command
        gameFlow->SetCurrentSystemLoopCommand(std::make_unique<PlaySceneWhenDeadSystemLoopCommand>());
        
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::RE_SPAWN,
        [entity, gameOverUIEntities](riaecs::IECSWorld& ecsWorld)
    {
        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlowController", RIAECS_LOG_LOC);

        // Set system loop command
        gameFlow->SetCurrentSystemLoopCommand(std::make_unique<PlaySceneWhenDeadSystemLoopCommand>());

        for (const riaecs::Entity& uiEntity : gameOverUIEntities)
        {
            // Get identity component
            mono_identity::ComponentIdentity* gameOverUIIdentity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, uiEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            // Show game over UI
            gameOverUIIdentity->SetActiveSelf(true);
        }
        
        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2);

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD_COUNT_REACHED, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_OVER,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PLAYER_DEAD_COUNT_REACHED event
        riaecs::Log::OutToConsole("Player dead count reached in PHASE_2 state. Game Over.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PAUSE, 
        (mono_cycle::State)bdc_game_flow::GameFlowState::MENU,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for PAUSE event
        riaecs::Log::OutToConsole("Game paused from PHASE_2 state. Entering MENU.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    }, (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2);

    /*******************************************************************************************************************
     * State transition from GAME_CLEAR state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_CLEAR, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::BACK_TO_TITLE, 
        mono_cycle::STATE_NULL,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for BACK_TO_TITLE event
        riaecs::Log::OutToConsole("Returning to title from GAME_CLEAR state.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    /*******************************************************************************************************************
     * State transition from GAME_OVER state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::GAME_OVER, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::BACK_TO_TITLE, 
        mono_cycle::STATE_NULL,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for BACK_TO_TITLE event
        riaecs::Log::OutToConsole("Returning to title from GAME_OVER state.\n");
        return mono_cycle::NextStateType::TO_NEXT;
    });

    /*******************************************************************************************************************
     * State transition from RE_SPAWN state
    /******************************************************************************************************************/

    stateMachine.AddTransition(
        (mono_cycle::State)bdc_game_flow::GameFlowState::RE_SPAWN, 
        (mono_cycle::Event)bdc_game_flow::GameFlowEvent::RE_SPAWN_COMPLETE, 
        mono_cycle::STATE_NULL,
        [](riaecs::IECSWorld& ecsWorld)
    {
        // Action for RE_SPAWN_COMPLETE event
        riaecs::Log::OutToConsole("Re-spawn complete.\n");
        return mono_cycle::NextStateType::TO_PREVIOUS;
    });

    /*******************************************************************************************************************
     * Initial event trigger
    /******************************************************************************************************************/

    stateMachine.HandleEvent((mono_cycle::Event)bdc_game_flow::GameFlowEvent::START, ecsWorld);
}

std::vector<riaecs::Entity> bdc::play_scene::CreatePlayingUI(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, const riaecs::Entity &playSceneEntity, 
    riaecs::StagingEntityArea &stagingArea, const nlohmann::json &configJson)
{
    // Prepare entity vector
    std::vector<riaecs::Entity> uiEntities;

    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    // Player bar
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "PlayerBarUI";
        identity->Setup(identityParam);

        // UI
        ecsWorld.AddComponent(entity, mono_render::UIComponentID());
        mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
            ecsWorld, entity, mono_render::UIComponentID());
        mono_render::UIComponent::SetupParam uiComponentParam;
        uiComponentParam.ui_drawer = std::make_unique<bdc_ui::ImageUIDrawer>(
            window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
            configJson["playing_ui"]["player_bar"]["position"]["x"].get<float>(),
            configJson["playing_ui"]["player_bar"]["position"]["y"].get<float>(),
            configJson["playing_ui"]["player_bar"]["size"]["x"].get<float>(),
            configJson["playing_ui"]["player_bar"]["size"]["y"].get<float>(),
            bdc::PlayerBarTextureAssetSourceID(), 
            assetCont, "PlayerBar");
        uiComponent->Setup(uiComponentParam);

        uiEntities.push_back(entity);
    }

    return uiEntities;
}

std::vector<riaecs::Entity> bdc::play_scene::CreateGameOverUI
(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity& playSceneEntity, riaecs::StagingEntityArea& stagingArea, 
    const nlohmann::json& configJson,
    const riaecs::Entity& loadingBackgroundEntity)
{
    // Prepare entity vector
    std::vector<riaecs::Entity> entities;

    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    // Background Image UI
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "GameOverUI_Background";
        identityParam.activeSelf = false; // Initially inactive
        identity->Setup(identityParam);

        // UI
        ecsWorld.AddComponent(entity, mono_render::UIComponentID());
        mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
            ecsWorld, entity, mono_render::UIComponentID());
        mono_render::UIComponent::SetupParam uiComponentParam;
        uiComponentParam.ui_drawer = std::make_unique<bdc_ui::ImageUIDrawer>(
            window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
            1920.0f / 2.0f, 
            1080.0f / 2.0f,
            1920.0f, 1080.0f,
            bdc::GameOverBackgroundTextureAssetSourceID(), assetCont, "GameOverUI_Background");
        uiComponent->Setup(uiComponentParam);

        entities.push_back(entity);
    }

    // Back to menu UI
    {
        riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "GameOverUI_BackToMenu";
        identityParam.activeSelf = false; // Initially inactive
        identity->Setup(identityParam);

        // UI
        ecsWorld.AddComponent(entity, mono_render::UIComponentID());
        mono_render::UIComponent* uiComponent = riaecs::GetComponent<mono_render::UIComponent>(
            ecsWorld, entity, mono_render::UIComponentID());
        mono_render::UIComponent::SetupParam uiComponentParam;
        uiComponentParam.ui_drawer = std::make_unique<bdc_ui::ButtonUIDrawer>(
            window->GetInfo().GetHandle(), 1920.0f, 1080.0f,
            configJson["game_over_ui"]["back_to_menu_button"]["text"].get<std::string>(),
            configJson["game_over_ui"]["back_to_menu_button"]["text_size"].get<int>(),
            DirectX::XMINT4(
                configJson["game_over_ui"]["back_to_menu_button"]["text_color"]["r"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["text_color"]["g"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["text_color"]["b"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["text_color"]["a"].get<int>()),
            DirectX::XMINT4(
                configJson["game_over_ui"]["back_to_menu_button"]["hover_text_color"]["r"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["hover_text_color"]["g"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["hover_text_color"]["b"].get<int>(),
                configJson["game_over_ui"]["back_to_menu_button"]["hover_text_color"]["a"].get<int>()),
            configJson["game_over_ui"]["back_to_menu_button"]["position"]["x"].get<float>(),
            configJson["game_over_ui"]["back_to_menu_button"]["position"]["y"].get<float>(),
            configJson["game_over_ui"]["back_to_menu_button"]["size"]["x"].get<float>(),
            configJson["game_over_ui"]["back_to_menu_button"]["size"]["y"].get<float>(),
            assetCont, "BackToMenuButton",
            [&ecsWorld, loadingBackgroundEntity, entity]()
            {
                // Get menu scene entity
                riaecs::Entity menuSceneEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MENU_SCENE);
                assert(menuSceneEntity.IsValid() && "Menu Scene entity is invalid!");

                // Get scene component
                mono_scene::ComponentScene* menuScene = riaecs::GetComponentWithCheck<mono_scene::ComponentScene>(
                    ecsWorld, menuSceneEntity, mono_scene::ComponentSceneID(), "Menu Scene", RIAECS_LOG_LOC);

                // Request load scene
                menuScene->NeedsLoadRW()() = true;
                menuScene->NeedsEditSystemListRW()() = true;

                // Get loading background's identity component
                mono_identity::ComponentIdentity* loadingBgIdentity = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, loadingBackgroundEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Show loading background
                loadingBgIdentity->SetActiveSelf(true);

                // Get start button's identity component
                mono_identity::ComponentIdentity* startButtonIdentity = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                // Hide start button
                startButtonIdentity->SetActiveSelf(false);
            });            
        uiComponent->Setup(uiComponentParam);

        entities.push_back(entity);
    }

    return entities;
}

riaecs::Entity bdc::play_scene::CreateCage(
    riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
    const riaecs::Entity& playSceneEntity, riaecs::StagingEntityArea& stagingArea, const nlohmann::json& configJson)
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

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get cage model asset
    riaecs::ROObject<riaecs::IAsset> cage_model_asset 
        = assetCont.Get({CageModelAssetSourceID(), assetCont.GetGeneration(CageModelAssetSourceID())});
    const mono_asset::AssetModel* cage_model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&cage_model_asset());
    assert(cage_model_asset_ptr != nullptr);

    {
        // Create material for cage
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
        material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        material_param->emission_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::CageLambertMaterialHandleID().ID());

        // Create Lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(bdc::CageLambertMaterialHandleID().ID()),
            std::move(material_param));  
    }

    riaecs::Entity cageEntity = ecsWorld.CreateEntity(stagingArea);

    // SceneTag
    ecsWorld.AddComponent(cageEntity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
        ecsWorld, cageEntity, mono_scene::ComponentSceneTagID());
    mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
    sceneTagParam.sceneEntity = playSceneEntity;
    sceneTag->Setup(sceneTagParam);

    // Identity
    ecsWorld.AddComponent(cageEntity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
        ecsWorld, cageEntity, mono_identity::ComponentIdentityID());
    mono_identity::ComponentIdentity::SetupParam identityParam;
    identityParam.name = "Cage";
    identity->Setup(identityParam);

    // Transform
    ecsWorld.AddComponent(cageEntity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
        ecsWorld, cageEntity, mono_transform::ComponentTransformID());
    mono_transform::ComponentTransform::SetupParam transformParam;
    {
        // Get the locator asset
        riaecs::ROObject<riaecs::IAsset> locatorAsset
            = assetCont.Get({ bdc::CageLocatorAssetSourceID(), assetCont.GetGeneration(bdc::CageLocatorAssetSourceID()) });
        const mono_asset::AssetLocator* locAsset
            = dynamic_cast<const mono_asset::AssetLocator*>(&locatorAsset());

        // Set the initial position from the asset
        const mono_asset::AssetLocator::LocatedTransform& firstTransform = locAsset->GetTransforms().front();
        transformParam.pos_ = firstTransform.translation_;
        transformParam.pitch_ = XMConvertToRadians(firstTransform.rotation_.x);
        transformParam.yaw_ = XMConvertToRadians(firstTransform.rotation_.y);
        transformParam.roll_ = XMConvertToRadians(firstTransform.rotation_.z);
        transformParam.scale_ = firstTransform.scale_;
    }
    transform->Setup(transformParam);

    // MeshRenderer
    ecsWorld.AddComponent(cageEntity, mono_render::ComponentMeshRendererID());
    mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
        ecsWorld, cageEntity, mono_render::ComponentMeshRendererID());
    mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
    meshRendererParam.vertex_buffer_handles = *cage_model_asset_ptr->GetVertexBufferHandles();
    meshRendererParam.index_buffer_handles = *cage_model_asset_ptr->GetIndexBufferHandles();
    meshRendererParam.index_counts = *cage_model_asset_ptr->GetIndexCounts();

    std::vector<const render_graph::MaterialHandle*> material_handles;
    material_handles.resize(cage_model_asset_ptr->GetIndexCounts()->size());
    for (int i = 0; i < material_handles.size(); ++i)
        material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::CageLambertMaterialHandleID().ID());
    meshRendererParam.material_handles = std::move(material_handles);
    meshRendererParam.cast_shadow = false;
    meshRenderer->Setup(meshRendererParam);

    std::vector<size_t> CageBoundingBoxAssetSourceIDs;
    CageBoundingBoxAssetSourceIDs.push_back(bdc::NorthCageBoundingBoxAssetSourceID());
    CageBoundingBoxAssetSourceIDs.push_back(bdc::SouthCageBoundingBoxAssetSourceID());
    CageBoundingBoxAssetSourceIDs.push_back(bdc::WestCageBoundingBoxAssetSourceID());
    CageBoundingBoxAssetSourceIDs.push_back(bdc::EastCageBoundingBoxAssetSourceID());

    // BoxCollider
    for (size_t i = 0; i < 4; i++)
    {
        riaecs::Entity colliderEntity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(colliderEntity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, colliderEntity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(colliderEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, colliderEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "CageCollider" + i;
        identity->Setup(identityParam);

        // RigidBody
        ecsWorld.AddComponent(colliderEntity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, colliderEntity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = true;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

        // Transform
        ecsWorld.AddComponent(colliderEntity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, colliderEntity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transform->Setup(transformParam);
        transform->SetParent(colliderEntity, cageEntity, ecsWorld);

        // Colider
        ecsWorld.AddComponent(colliderEntity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, colliderEntity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = false;
        {
            // Get the bounding box asset
            riaecs::ROObject<riaecs::IAsset> boundingBoxAsset
                = assetCont.Get({ CageBoundingBoxAssetSourceIDs[i], assetCont.GetGeneration(CageBoundingBoxAssetSourceIDs[i])});
            const mono_asset::AssetBoundingBox* boxAsset
                = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());

            // Set the box shape from the asset
            boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
            boxColliderParam.box->SetMin(boxAsset->GetMin());
            boxColliderParam.box->SetMax(boxAsset->GetMax());
        }
        boxCollider->Setup(boxColliderParam);
        boxCollider->AddCollidableComponentID(mono_physics::ComponentBoxColliderID()); // Can collide with other box colliders

        // Cage child
        ecsWorld.AddComponent(colliderEntity, bdc_cage::ComponentCageChildID());
        bdc_cage::ComponentCageChild* cageChild = riaecs::GetComponent<bdc_cage::ComponentCageChild>(
            ecsWorld, colliderEntity, bdc_cage::ComponentCageChildID());
        bdc_cage::ComponentCageChild::SetupParam cageChildParam;
        cageChild->Setup(cageChildParam);
    }

    // Cage
    ecsWorld.AddComponent(cageEntity, bdc_cage::ComponentCageID());
    bdc_cage::ComponentCage* cage = riaecs::GetComponent<bdc_cage::ComponentCage>(
        ecsWorld, cageEntity, bdc_cage::ComponentCageID());

    bdc_cage::ComponentCage::SetupParam cageParam;
    cageParam.moveSpeed_ = configJson["cage"]["phase_0_move_speed"].get<float>();
    cageParam.cageSize_ = 35.0f;
    cageParam.targetPos_ = DirectX::XMFLOAT3(20,0,20);
    cageParam.roamCount_ = 2;
    cageParam.perCellDelay = 2.0f;
    cageParam.numDivisions_ = 8;
    cage->Setup(cageParam);

    // Submit graphics command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

    return cageEntity;
}

std::vector<riaecs::Entity> bdc::play_scene::CreateCrystals
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
)
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

    // Get the bounding box asset
    riaecs::ROObject<riaecs::IAsset> boundingBoxAsset 
        = assetCont.Get({CrystalBoundingBoxAssetSourceID(), assetCont.GetGeneration(CrystalBoundingBoxAssetSourceID())});
    const mono_asset::AssetBoundingBox* boxAsset
        = dynamic_cast<const mono_asset::AssetBoundingBox*>(&boundingBoxAsset());
    assert(boxAsset != nullptr);

    // Get empty texture asset
    riaecs::ROObject<riaecs::IAsset> empty_texture_asset 
        = assetCont.Get({EmptyTextureAssetSourceID(), assetCont.GetGeneration(EmptyTextureAssetSourceID())});
    const mono_asset::AssetTexture* empty_texture_asset_ptr = dynamic_cast<const mono_asset::AssetTexture*>(&empty_texture_asset());
    assert(empty_texture_asset_ptr != nullptr);

    // Get model asset
    riaecs::ROObject<riaecs::IAsset> model_asset
        = assetCont.Get({bdc::CrystalModelAssetSourceID(), assetCont.GetGeneration(bdc::CrystalModelAssetSourceID())});
    const mono_asset::AssetModel* model_asset_ptr = dynamic_cast<const mono_asset::AssetModel*>(&model_asset());
    assert(model_asset_ptr != nullptr);

    {
        // Create material for crystal
        std::unique_ptr<render_graph::PhongMaterial::SetupParam> material_param
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        material_param->base_color = DirectX::XMFLOAT4(0.0f, 125.0f / 255.0f, 197.0f / 255.0f, 1.0f);
        material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_BASE_COLOR;
        material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_VERTEX;
        material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_NONE;
        material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_VALUE;
        material_param->roughness_value = 1.0f;
        material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        material_param->metalness_value = 0.0f;
        material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        material_param->emission_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->roughness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(bdc::CrystalPhongMaterialHandleID().ID());

        // Create Phong material
        graphics_command_list_ptr->CreateMaterial<render_graph::PhongMaterial>(
            material_handle_manager.GetMaterialHandle(bdc::CrystalPhongMaterialHandleID().ID()),
            std::move(material_param));
    }

    std::vector<riaecs::Entity> crystalEntities;

    size_t crystalCount = 0;
    for (const auto& crystalJson : configJson["crystals"])
    {
        riaecs::Entity crystalEntity = ecsWorld.CreateEntity(stagingArea);

        // SceneTag
        ecsWorld.AddComponent(crystalEntity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag* sceneTag = riaecs::GetComponent<mono_scene::ComponentSceneTag>(
            ecsWorld, crystalEntity, mono_scene::ComponentSceneTagID());
        mono_scene::ComponentSceneTag::SetupParam sceneTagParam;
        sceneTagParam.sceneEntity = playSceneEntity;
        sceneTag->Setup(sceneTagParam);

        // Identity
        ecsWorld.AddComponent(crystalEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity* identity = riaecs::GetComponent<mono_identity::ComponentIdentity>(
            ecsWorld, crystalEntity, mono_identity::ComponentIdentityID());
        mono_identity::ComponentIdentity::SetupParam identityParam;
        identityParam.name = "Crystal" + std::to_string(crystalCount);
        identityParam.activeSelf = false; // Initially inactive
        identity->Setup(identityParam);

        // Transform
        ecsWorld.AddComponent(crystalEntity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform* transform = riaecs::GetComponent<mono_transform::ComponentTransform>(
            ecsWorld, crystalEntity, mono_transform::ComponentTransformID());
        mono_transform::ComponentTransform::SetupParam transformParam;
        transformParam.pos_ = DirectX::XMFLOAT3(
            crystalJson["spawn_point"]["x"].get<float>(),
            crystalJson["spawn_point"]["y"].get<float>(),
            crystalJson["spawn_point"]["z"].get<float>());
        transformParam.scale_ = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
        transform->Setup(transformParam);

        // MeshRenderer
        ecsWorld.AddComponent(crystalEntity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer* meshRenderer = riaecs::GetComponent<mono_render::ComponentMeshRenderer>(
            ecsWorld, crystalEntity, mono_render::ComponentMeshRendererID());
        mono_render::ComponentMeshRenderer::SetupParam meshRendererParam;
        meshRendererParam.vertex_buffer_handles = *model_asset_ptr->GetVertexBufferHandles();
        meshRendererParam.index_buffer_handles = *model_asset_ptr->GetIndexBufferHandles();
        meshRendererParam.index_counts = *model_asset_ptr->GetIndexCounts();

        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(model_asset_ptr->GetIndexCounts()->size());
        for (int i = 0; i < material_handles.size(); ++i)
            material_handles[i] = material_handle_manager.GetMaterialHandle(bdc::CrystalPhongMaterialHandleID().ID());
        meshRendererParam.material_handles = std::move(material_handles);
        meshRendererParam.cast_shadow = true;
        meshRenderer->Setup(meshRendererParam);

        // BoxCollider
        ecsWorld.AddComponent(crystalEntity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider* boxCollider = riaecs::GetComponent<mono_physics::ComponentBoxCollider>(
            ecsWorld, crystalEntity, mono_physics::ComponentBoxColliderID());
        mono_physics::ComponentBoxCollider::SetupParam boxColliderParam;
        boxColliderParam.isTrigger = true;
        boxColliderParam.box = std::make_unique<mono_physics::ShapeBox>();
        boxColliderParam.box->SetMin(boxAsset->GetMin());
        boxColliderParam.box->SetMax(boxAsset->GetMax());
        boxCollider->Setup(boxColliderParam);

        // RigidBody
        ecsWorld.AddComponent(crystalEntity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody* rigidBody = riaecs::GetComponent<mono_physics::ComponentRigidBody>(
            ecsWorld, crystalEntity, mono_physics::ComponentRigidBodyID());
        mono_physics::ComponentRigidBody::SetupParam rigidBodyParam;
        rigidBodyParam.isStatic = true;
        rigidBody->Setup(rigidBodyParam);
        rigidBody->SetAttachedColliderComponentID(mono_physics::ComponentBoxColliderID());

        // Crystal Component
        ecsWorld.AddComponent(crystalEntity, bdc_interactive_object::ComponentCrystalID());
        bdc_interactive_object::ComponentCrystal* crystal = riaecs::GetComponent<bdc_interactive_object::ComponentCrystal>(
            ecsWorld, crystalEntity, bdc_interactive_object::ComponentCrystalID());
        bdc_interactive_object::ComponentCrystal::SetupParam crystalParam;
        crystal->Setup(crystalParam);

        crystalEntities.push_back(crystalEntity);
        crystalCount++;
    }

    // Submit graphics command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

    return crystalEntities;
}
