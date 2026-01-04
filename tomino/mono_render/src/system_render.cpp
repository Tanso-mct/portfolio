#include "mono_render/src/pch.h"
#include "mono_render/include/system_render.h"

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_d3d12.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_scene.lib")

#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service_view.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "render_graph/include/directional_light.h"
#include "render_graph/include/ambient_light.h"
#include "render_graph/include/point_light.h"
#include "render_graph/include/lighting_pass.h"

#include "mono_render/include/component_camera.h"
#include "mono_render/include/component_mesh_renderer.h"
#include "mono_render/include/ui_component.h"
#include "mono_render/include/light_component.h"
#include "mono_render/include/ambient_light_component.h"
#include "mono_render/include/directional_light_component.h"
#include "mono_render/include/point_light_component.h"

#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"

namespace mono_render
{
struct RenderContext
{
    RenderContext(
        riaecs::IECSWorld& arg_world,
        mono_scene::ComponentScene* arg_scene_component,
        std::unique_ptr<mono_service::ServiceProxy> arg_graphics_service_proxy,
        const riaecs::Entity& arg_window_entity) :
        world(arg_world),
        scene_component(arg_scene_component),
        graphics_service_proxy(std::move(arg_graphics_service_proxy)),
        window_entity(arg_window_entity)
    {
    }

    riaecs::IECSWorld& world;
    mono_scene::ComponentScene* scene_component;
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy;
    riaecs::Entity window_entity;

    // Main camera entity in the scene
    riaecs::Entity main_camera_entity = riaecs::Entity();

    // Mesh renderer entities in the scene
    std::vector<riaecs::Entity> mesh_renderer_entities = {};

    // Light entities in the scene
    // First: LightComponent ID, Second: Entity
    std::vector<std::pair<size_t, riaecs::Entity>> light_entities = {};

    // UI entities in the scene
    // First: UIComponent ID, Second: Entity
    std::vector<std::pair<size_t, riaecs::Entity>> ui_entities = {};
};

// Helper function to get current back buffer index
uint32_t GetCurrentBackBufferIndex(
    mono_service::ServiceProxy& graphics_service_proxy,
    const render_graph::ResourceHandle* swap_chain_handle)
{
    // Create graphics service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = graphics_service_proxy.CreateView();
    mono_graphics_service::GraphicsServiceView* graphics_service_view
        = dynamic_cast<mono_graphics_service::GraphicsServiceView*>(service_view.get());
    assert(graphics_service_view != nullptr && "Failed to create graphics service view");

    // Get current back buffer index
    return graphics_service_view->GetCurrentBackBufferIndex(swap_chain_handle);
}

// Update mesh renderer components in the scene
void UpdateMeshRenderer(RenderContext& draw_context, const render_graph::CommandSetHandle* command_set_handle)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Iterate through all mesh renderers in the scene
    for (const riaecs::Entity& mesh_renderer_entity : draw_context.mesh_renderer_entities)
    {
        // Get mesh renderer component
        mono_render::ComponentMeshRenderer* mesh_renderer_component
            = riaecs::GetComponentWithCheck<mono_render::ComponentMeshRenderer>(
                draw_context.world, mesh_renderer_entity, mono_render::ComponentMeshRendererID(), "MeshRenderer", RIAECS_LOG_LOC);

        // Get transform component
        mono_transform::ComponentTransform* transform_component
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            draw_context.world, mesh_renderer_entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

        // Create world buffer for geometry pass
        std::unique_ptr<render_graph::geometry_pass::WorldBuffer> world_buffer
            = std::make_unique<render_graph::geometry_pass::WorldBuffer>();
        world_buffer->world_matrix = XMMatrixTranspose(transform_component->GetWorldMatrix());
        world_buffer->world_inverse_transpose
            = XMMatrixTranspose(XMMatrixInverse(nullptr, transform_component->GetWorldMatrix()));

        // Update world buffer in graphics service
        graphics_command_list->UpdateWorldBufferForGeometryPass(
            mesh_renderer_component->GetWorldMatrixBufferHandle(), std::move(world_buffer));
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy->SubmitCommandList(std::move(command_list));
}

// Update view-projection matrix from main camera in the scene
void UpdateCamera(RenderContext& draw_context, const render_graph::CommandSetHandle* command_set_handle)
{
    // Get main camera entity
    const riaecs::Entity& main_camera_entity = draw_context.main_camera_entity;

    // Get CameraComponent
    mono_render::ComponentCamera* camera_component
        = riaecs::GetComponentWithCheck<mono_render::ComponentCamera>(
            draw_context.world, main_camera_entity, mono_render::ComponentCameraID(), "Camera", RIAECS_LOG_LOC);

    // Get transform component
    mono_transform::ComponentTransform* transform_component
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            draw_context.world, main_camera_entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

    // Get trs
    XMFLOAT3 translation = transform_component->GetPos();
    XMFLOAT4 rotation = transform_component->GetRotByQuat();

    // Calculate view matrix
    XMVECTOR up_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR rot_vec = XMLoadFloat4(&rotation);
    XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
    XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);
    XMVECTOR position_vec = XMLoadFloat3(&translation);

    XMMATRIX view_matrix = XMMatrixLookAtLH(
        position_vec, XMVectorAdd(position_vec, rotated_forward_vec), rotated_up_vec);

    // Calculate projection matrix
    XMMATRIX projection_matrix = XMMatrixPerspectiveFovLH(
        camera_component->GetFovY(), camera_component->GetAspectRatio(), 
        camera_component->GetNearZ(), camera_component->GetFarZ());

    // Combine view and projection matrix
    XMMATRIX view_proj_matrix = XMMatrixMultiply(view_matrix, projection_matrix);

    // Create inverse view-projection matrix
    XMMATRIX inv_view_proj_matrix = XMMatrixInverse(nullptr, view_proj_matrix);

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Update view-projection matrix buffer in graphics service
    std::unique_ptr<XMMATRIX> view_proj_mat_data 
        = std::unique_ptr<XMMATRIX>(new XMMATRIX(XMMatrixTranspose(view_proj_matrix)));
    graphics_command_list->UpdateMatrixBuffer(
        camera_component->GetViewProjMatrixBufferHandle(), std::move(view_proj_mat_data));

    // Update inverse view-projection matrix buffer in graphics service
    std::unique_ptr<XMMATRIX> inv_view_proj_mat_data 
        = std::unique_ptr<XMMATRIX>(new XMMATRIX(XMMatrixTranspose(inv_view_proj_matrix)));
    graphics_command_list->UpdateMatrixBuffer(
        camera_component->GetInvViewProjMatrixBufferHandle(), std::move(inv_view_proj_mat_data));

    // Submit command list to graphics service
    draw_context.graphics_service_proxy->SubmitCommandList(std::move(command_list));
}

void UpdateLight(
    RenderContext& draw_context, 
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle,
    mono_d3d12::WindowRenderBindComponent* window_render_bind_component,
    uint32_t current_back_buffer_index, uint32_t client_width, uint32_t client_height)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Iterate through all light entities in the scene
    for (const auto& [component_id, light_entity] : draw_context.light_entities)
    {
        // Get LightComponent
        LightComponent* light_component
            = riaecs::GetComponentWithCheck<LightComponent>(
                draw_context.world, light_entity, component_id, "LightComponent", RIAECS_LOG_LOC);

        // Upload light to graphics service
        graphics_command_list->UploadLight(light_component->GetLightHandle());

        // Get transform component
        mono_transform::ComponentTransform* transform_component
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                draw_context.world, light_entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

        // Update light view-projection matrix
        graphics_command_list->UpdateLightViewProjMatrixBuffer(
            light_component->GetLightHandle(),
            std::make_unique<XMFLOAT3>(transform_component->GetPos()),
            std::make_unique<XMFLOAT3>(transform_component->GetRotByEuler()));

        // Get light setup parameters
        std::unique_ptr<render_graph::Light::SetupParam> resource_setup_param = light_component->GetLightSetupParam();

        // Update light buffer in graphics service
        graphics_command_list->UpdateLightBuffer(
            light_component->GetLightHandle(), std::move(resource_setup_param));

        // If the light casts shadows, render shadow map
        if (light_component->CastShadow())
            graphics_command_list->CastShadow(light_component->GetLightHandle());
    }

    // Update lights buffer in graphics service
    graphics_command_list->UpdateLightsBuffer(
        &window_render_bind_component->GetLightsBufferHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetLightsUploadBufferHandles()->at(current_back_buffer_index), 
        draw_context.scene_component->GetMaxLightCount());

    {
        // Create shadow composition config buffer data
        std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> shadow_comp_config
            = std::make_unique<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer>();
        shadow_comp_config->screen_size.x = client_width;
        shadow_comp_config->screen_size.y = client_height;
        shadow_comp_config->shadow_bias = draw_context.scene_component->GetShadowBias();
        shadow_comp_config->slope_scaled_bias
            = draw_context.scene_component->GetShadowSlopeScaledBias();
        shadow_comp_config->slope_bias_exponent
            = draw_context.scene_component->GetShadowSlopeBiasExponent();

        // Update shadow composition config buffer in graphics service
        graphics_command_list->UpdateShadowCompositionConfigBuffer(
            window_render_bind_component->GetShadowCompositionConfigBufferHandle(), std::move(shadow_comp_config));
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy->SubmitCommandList(std::move(command_list));
}

// Draw entities in the scene
void DrawEntities(RenderContext& draw_context, 
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Iterate through all renderable entities in the scene
    for (const riaecs::Entity& mesh_renderer_entity : draw_context.mesh_renderer_entities)
    {
        // Get mesh renderer component
        ComponentMeshRenderer* mesh_renderer_component
            = riaecs::GetComponentWithCheck<ComponentMeshRenderer>(
                draw_context.world, mesh_renderer_entity, ComponentMeshRendererID(), "MeshRenderer", RIAECS_LOG_LOC);

        for (uint32_t i = 0; i < mesh_renderer_component->GetIndexCounts()->size(); ++i)
        {
            // Add draw mesh command
            graphics_command_list->DrawMesh(
                mesh_renderer_component->GetWorldMatrixBufferHandle(),
                mesh_renderer_component->GetMaterialHandles().at(i),
                &mesh_renderer_component->GetVertexBufferHandles()->at(i),
                &mesh_renderer_component->GetIndexBufferHandles()->at(i),
                mesh_renderer_component->GetIndexCounts()->at(i));

            if (mesh_renderer_component->NeedsCastShadow())
            {
                // Add draw mesh to shadow map command
                graphics_command_list->DrawShadowCasterMesh(
                    mesh_renderer_component->GetWorldMatrixBufferHandle(),
                    &mesh_renderer_component->GetVertexBufferHandles()->at(i),
                    &mesh_renderer_component->GetIndexBufferHandles()->at(i),
                    mesh_renderer_component->GetIndexCounts()->at(i));
            }
        }
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy->SubmitCommandList(std::move(command_list));
}

void AddRenderPasses(
    RenderContext& draw_context,
    uint32_t current_back_buffer_index, uint32_t client_width, uint32_t clint_height,
    mono_d3d12::WindowRenderBindComponent* window_render_bind_component,
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle,
    render_graph::ImguiPass::DrawFunc ui_draw_func)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Get main camera entity
    const riaecs::Entity& main_camera_entity = draw_context.main_camera_entity;

    // Get CameraComponent
    mono_render::ComponentCamera* camera_component
        = riaecs::GetComponentWithCheck<mono_render::ComponentCamera>(
            draw_context.world, main_camera_entity, mono_render::ComponentCameraID(), "Camera", RIAECS_LOG_LOC);

    // Get camera transform component
    mono_transform::ComponentTransform* camera_transform_component
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            draw_context.world, main_camera_entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Add light upload pass to graph
    render_graph::Light::LightConfigBuffer light_config;
    light_config.screen_size.x = client_width;
    light_config.screen_size.y = clint_height;
    light_config.max_lights = draw_context.scene_component->GetMaxLightCount();
    light_config.shadow_intensity = draw_context.scene_component->GetShadowIntensity();
    light_config.camera_world_matrix = XMMatrixTranspose(camera_transform_component->GetWorldMatrix());
    light_config.lambert_material_id = render_graph::LambertMaterialTypeHandle::ID();
    light_config.phong_material_id = render_graph::PhongMaterialTypeHandle::ID();
    light_config.directional_light_id = render_graph::DirectionalLightTypeHandle::ID();
    light_config.ambient_light_id = render_graph::AmbientLightTypeHandle::ID();
    light_config.point_light_id = render_graph::PointLightTypeHandle::ID();

    // Add light upload pass to render graph
    graphics_command_list->AddLightUploadPassToGraph(
        &window_render_bind_component->GetLightsUploadBufferHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetLightConfigBufferHandle(), light_config);

    // Add buffer update pass to render graph
    graphics_command_list->AddBufferUploadPassToGraph();

    // Add texture upload pass to render graph
    graphics_command_list->AddTextureUploadPassToGraph();

    // Add shadowing pass to render graph
    graphics_command_list->AddShadowingPassToGraph(current_back_buffer_index);

    // Add geometry pass to render graph
    graphics_command_list->AddGeometryPassToGraph(
        &window_render_bind_component->GetGBufferTextureHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetGDepthStencilTextureHandles()->at(current_back_buffer_index),
        camera_component->GetViewProjMatrixBufferHandle(),
        window_render_bind_component->GetViewPort(), window_render_bind_component->GetScissorRect());

    // Add Imgui pass to render graph
    graphics_command_list->AddImguiPassToGraph(
        &window_render_bind_component->GetImguiRenderTargetTextureHandles()->at(current_back_buffer_index),
        std::move(ui_draw_func), window_render_bind_component->GetImguiContextHandle()); 

    // Add shadow composition pass to render graph
    graphics_command_list->AddShadowCompositionPassToGraph(
        &window_render_bind_component->GetShadowCompositionRenderTargetTextureHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetShadowCompositionConfigBufferHandle(),
        camera_component->GetInvViewProjMatrixBufferHandle(),
        &window_render_bind_component->GetGDepthStencilTextureHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetGBufferTextureHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetFullScreenTriangleVertexBufferHandle(),
        window_render_bind_component->GetFullScreenTriangleIndexBufferHandle(),
        window_render_bind_component->GetViewPort(), window_render_bind_component->GetScissorRect());

    // Add lighting pass to render graph
    graphics_command_list->AddLightingPassToGraph(
        &window_render_bind_component->GetLightingRenderTargetTextureHandles()->at(current_back_buffer_index),
        camera_component->GetInvViewProjMatrixBufferHandle(),
        window_render_bind_component->GetLightConfigBufferHandle(),
        &window_render_bind_component->GetLightsBufferHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetGDepthStencilTextureHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetGBufferTextureHandles()->at(current_back_buffer_index),
        &window_render_bind_component->GetShadowCompositionRenderTargetTextureHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetFullScreenTriangleVertexBufferHandle(),
        window_render_bind_component->GetFullScreenTriangleIndexBufferHandle(),
        window_render_bind_component->GetViewPort(), window_render_bind_component->GetScissorRect());

    // Add composition pass to render graph
    graphics_command_list->AddCompositionPassToGraph(
        swap_chain_handle, draw_context.scene_component->GetClearColor(),
        &window_render_bind_component->GetLightingRenderTargetTextureHandles()->at(current_back_buffer_index)
        [(uint32_t)render_graph::lighting_pass::RenderTargetIndex::FINAL_COLOR],
        &window_render_bind_component->GetImguiRenderTargetTextureHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetFullScreenTriangleVertexBufferHandle(),
        window_render_bind_component->GetFullScreenTriangleIndexBufferHandle(),
        window_render_bind_component->GetViewPort(), window_render_bind_component->GetScissorRect());

    // Submit command list to graphics service
    draw_context.graphics_service_proxy->SubmitCommandList(std::move(command_list));
}

render_graph::ImguiPass::DrawFunc DrawUI(RenderContext& draw_context)
{
    // List of UI draw functions
    std::vector<render_graph::ImguiPass::DrawFunc> ui_draw_funcs;

    // Iterate through all UI entities in the scene
    for (const auto& [component_id, ui_entity] : draw_context.ui_entities)
    {
        UIComponent* ui_component
            = riaecs::GetComponentWithCheck<UIComponent>(
                draw_context.world, ui_entity, component_id, "UIComponent", RIAECS_LOG_LOC);

        // Create draw function for ImGui pass
        render_graph::ImguiPass::DrawFunc draw_func = ui_component->CreateDrawFunc();
        assert(draw_func != nullptr && "UI component returned null draw func");

        // Add to the list
        ui_draw_funcs.push_back(std::move(draw_func));
    }

    return [ui_draw_funcs = std::move(ui_draw_funcs)](ImGuiContext* imgui_context)
    {
        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        // Call UI draw functions from components
        for (const auto& draw_func : ui_draw_funcs)
            draw_func(imgui_context);

        return true; // Success
    };
}

// Draw the scene
void DrawScene(RenderContext& draw_context)
{
    // Get window entity associated with this scene
    const riaecs::Entity& window_entity = draw_context.window_entity;
    
    // Get WindowComponent
    mono_d3d12::ComponentWindowD3D12* window_component
        = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
            draw_context.world, window_entity, mono_d3d12::ComponentWindowD3D12ID(), "WindowD3D12", RIAECS_LOG_LOC);

    // Get WindowRenderBindComponent
    mono_d3d12::WindowRenderBindComponent* window_render_bind_component
        = riaecs::GetComponentWithCheck<mono_d3d12::WindowRenderBindComponent>(
            draw_context.world, window_entity, mono_d3d12::WindowRenderBindComponentID(), "WindowRenderBindComponent", RIAECS_LOG_LOC);

    // If bind resources are not created, create them and continue
    if (!window_render_bind_component->IsBindResourcesCreated())
    {
        window_render_bind_component->CreateBindResources(
            window_component->GetInfo().GetHandle(),
            window_component->GetInfo().GetClientWidth(), window_component->GetInfo().GetClientHeight(),
            mono_render::BACK_BUFFER_COUNT, draw_context.scene_component->GetMaxLightCount());

        return; // Skip drawing this frame
    }

    // Get current back buffer index
    uint32_t current_back_buffer_index
        = GetCurrentBackBufferIndex(
            *draw_context.graphics_service_proxy, window_render_bind_component->GetSwapChainHandle());

    // Get command set handle for current frame
    const render_graph::CommandSetHandle* command_set_handle
        = &window_render_bind_component->GetCommandSetHandles()->at(current_back_buffer_index);

    // Update world matrixes
    UpdateMeshRenderer(draw_context, command_set_handle);

    // Update view-projection matrixes
    UpdateCamera(draw_context, command_set_handle);

    // Update lights
    UpdateLight(
        draw_context, command_set_handle, window_render_bind_component->GetSwapChainHandle(),
        window_render_bind_component, current_back_buffer_index,
        window_component->GetInfo().GetClientWidth(),
        window_component->GetInfo().GetClientHeight());

    // Draw all renderable entities in the scene
    DrawEntities(draw_context, command_set_handle, window_render_bind_component->GetSwapChainHandle());

    // Draw UI
    render_graph::ImguiPass::DrawFunc ui_draw_func = DrawUI(draw_context);

    // Add render passes to render graph
    AddRenderPasses(
        draw_context, current_back_buffer_index, 
        window_component->GetInfo().GetClientWidth(),
        window_component->GetInfo().GetClientHeight(),
        window_render_bind_component,
        command_set_handle, window_render_bind_component->GetSwapChainHandle(), std::move(ui_draw_func));
}

} // namespace mono_render

mono_render::SystemRender::SystemRender()
{
    // Create material handle ID generator
    material_handle_id_generator_ = std::make_unique<MaterialHandleIDGenerator>();

    // Create material handle manager
    material_handle_manager_ = std::make_unique<MaterialHandleManager>();

    // Create service registry
    service_registry_ = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service
    mono_graphics_service::GraphicsService::SetupParam setup_param;
    setup_param.srv_descriptor_count = 200;
    setup_param.rtv_descriptor_count = 200;
    setup_param.dsv_descriptor_count = 200;
    mono_service::ImportService<mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
        *service_registry_, 0, setup_param);

    service_registry_->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Initialize service adapter singleton
        graphics_service_adapter_ = std::make_unique<mono_adapter::GraphicsServiceAdapter>(
            registry.Get(mono_graphics_service::GraphicsServiceHandle::ID()));
    });
}

mono_render::SystemRender::~SystemRender()
{
    
}

bool mono_render::SystemRender::Update(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue)
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();

    // Iterate through all entities with WindowComponent
    std::unordered_map<riaecs::Entity, riaecs::Entity> window_to_scene_map;
    for (const riaecs::Entity& entity : ecsWorld.View(mono_d3d12::ComponentWindowD3D12ID())())
    {
        // Get ComponentIdentity
        mono_identity::ComponentIdentity* identity_component
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        if (identity_component->IsActiveSelf() == false)
            continue; // Skip inactive windows

        // Get WindowComponent
        mono_d3d12::ComponentWindowD3D12* window_component
            = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
                ecsWorld, entity, mono_d3d12::ComponentWindowD3D12ID(), "WindowD3D12", RIAECS_LOG_LOC);

        // Store mapping from window entity to scene entity
        window_to_scene_map[entity] = window_component->GetSceneEntity();
    }

    // Iterate through all entities with CameraComponent
    std::unordered_map<riaecs::Entity, riaecs::Entity> scene_to_camera_map;
    for (const riaecs::Entity& entity : ecsWorld.View(mono_render::ComponentCameraID())())
    {
        // Get ComponentIdentity
        mono_identity::ComponentIdentity* identity_component
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        if (identity_component->IsActiveSelf() == false)
            continue; // Skip inactive cameras

        // Get ComponentSceneTag
        mono_scene::ComponentSceneTag* scene_tag_component
            = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
                ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

        // If scene has a main camera already, log warning and skip
        if (scene_to_camera_map.find(scene_tag_component->GetSceneEntity()) != scene_to_camera_map.end())
        {
            riaecs::Log::OutToConsole(
                "Warning: Multiple main cameras found in scene entity", riaecs::CONSOLE_TEXT_COLOR_WARNING);
            continue; // Skip this camera
        }

        // Store mapping from scene entity to main camera entity
        scene_to_camera_map[scene_tag_component->GetSceneEntity()] = entity;
    }

    // Iterate through all entities with ComponentMeshRenderer
    std::unordered_map<riaecs::Entity, std::vector<riaecs::Entity>> scene_to_mesh_renderers_map;
    for (const riaecs::Entity& entity : ecsWorld.View(mono_render::ComponentMeshRendererID())())
    {
        // Get ComponentIdentity
        mono_identity::ComponentIdentity* identity_component
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        if (identity_component->IsActiveSelf() == false)
            continue; // Skip inactive mesh renderers

        // Get ComponentSceneTag
        mono_scene::ComponentSceneTag* scene_tag_component
            = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
                ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

        // Store mesh renderer entity under its scene entity
        scene_to_mesh_renderers_map[scene_tag_component->GetSceneEntity()].push_back(entity);
    }

    // Prepare all light component ids
    std::vector<size_t> light_component_ids;
    light_component_ids.push_back(mono_render::DirectionalLightComponentID());
    light_component_ids.push_back(mono_render::PointLightComponentID());
    light_component_ids.push_back(mono_render::AmbientLightComponentID());

    // Iterate through all entities with LightComponent
    std::unordered_map<riaecs::Entity, std::vector<std::pair<size_t, riaecs::Entity>>> scene_to_lights_map;
    for (size_t component_id : light_component_ids)
    {
        for (const riaecs::Entity& entity : ecsWorld.View(component_id)())
        {
            // Get ComponentIdentity
            mono_identity::ComponentIdentity* identity_component
                = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                    ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

            if (identity_component->IsActiveSelf() == false)
                continue; // Skip inactive lights

            // Get ComponentSceneTag
            mono_scene::ComponentSceneTag* scene_tag_component
                = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
                    ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

            // Store light entity under its scene entity
            scene_to_lights_map[scene_tag_component->GetSceneEntity()].emplace_back(component_id, entity);
        }
    }

    // Iterate through all entities with UIComponent
    std::unordered_map<riaecs::Entity, std::vector<std::pair<size_t, riaecs::Entity>>> scene_to_ui_map;
    for (const riaecs::Entity& entity : ecsWorld.View(mono_render::UIComponentID())())
    {
        // Get ComponentIdentity
        mono_identity::ComponentIdentity* identity_component
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        if (identity_component->IsActiveSelf() == false)
            continue; // Skip inactive UI components

        // Get ComponentSceneTag
        mono_scene::ComponentSceneTag* scene_tag_component
            = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
                ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

        // Store UI entity under its scene entity
        scene_to_ui_map[scene_tag_component->GetSceneEntity()].emplace_back(mono_render::UIComponentID(), entity);
    }

    // Iterate through all window to scene mappings
    for (const auto& [window_entity, scene_entity] : window_to_scene_map)
    {
        // If scene entity does not have a main camera, skip
        if (scene_to_camera_map.find(scene_entity) == scene_to_camera_map.end())
            continue; // Skip this scene

        // Get ComponentScene
        mono_scene::ComponentScene* scene_component
            = riaecs::GetComponentWithCheck<mono_scene::ComponentScene>(
                ecsWorld, scene_entity, mono_scene::ComponentSceneID(), "Scene", RIAECS_LOG_LOC);

        // Create graphics service proxy
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
            = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

        // Prepare render context
        RenderContext render_context(
            ecsWorld, scene_component, std::move(graphics_service_proxy), window_entity);
        render_context.main_camera_entity = scene_to_camera_map[scene_entity];
        render_context.mesh_renderer_entities = scene_to_mesh_renderers_map[scene_entity];
        render_context.light_entities = scene_to_lights_map[scene_entity];
        render_context.ui_entities = scene_to_ui_map[scene_entity];

        // Draw the scene
        DrawScene(render_context);
    }

    // Update graphics service
    bool result = true;
    service_registry_->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        if (!result)
            return;

        // Update
        result = service.Update();
        if (!result)
            return;

        // Post-update
        result = service.PostUpdate();
        if (!result)
            return;
    });

    if (!result)
		riaecs::NotifyError({ "Failed to update GraphicsService" }, RIAECS_LOG_LOC);

    return result;
}

MONO_RENDER_API riaecs::SystemFactoryRegistrar<mono_render::SystemRender> mono_render::SystemRenderID;