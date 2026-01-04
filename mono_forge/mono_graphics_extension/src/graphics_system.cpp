#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/graphics_system.h"

#include "ecs/include/world.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service_view.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_transform_extension/include/transform_component.h"
#include "mono_window_extension/include/window_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_scene_extension/include/scene_component.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "render_graph/include/directional_light.h"
#include "render_graph/include/ambient_light.h"
#include "render_graph/include/point_light.h"
#include "render_graph/include/lighting_pass.h"

#include "mono_graphics_extension/include/window_render_bind_component.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/light_component.h"

namespace mono_graphics_extension
{

// The context for drawing a scene
struct RenderContext
{
    RenderContext(
        ecs::World& world,
        mono_service::ServiceProxy& graphics_service_proxy,
        const ecs::Entity& scene_entity,
        const mono_scene_extension::SceneComponent& scene_component) :
        world(world),
        graphics_service_proxy(graphics_service_proxy),
        scene_entity(scene_entity),
        scene_component(scene_component)
    {
    }

    ecs::World& world;
    mono_service::ServiceProxy& graphics_service_proxy;
    const ecs::Entity& scene_entity;
    const mono_scene_extension::SceneComponent& scene_component;
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

// Update renderable components in the scene
void UpdateRenderable(RenderContext& draw_context, const render_graph::CommandSetHandle* command_set_handle)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy.CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Iterate through all renderable entities in the scene
    for (const ecs::Entity& renderable_entity : draw_context.scene_component.GetRenderableEntities())
    {
        RenderableComponent* renderable_component
            = draw_context.world.GetComponent<RenderableComponent>(
                renderable_entity, RenderableComponentHandle::ID());
        assert(renderable_component != nullptr && "Renderable component is null");

        // Get TransformComponent
        mono_transform_extension::TransformComponent* transform_component
            = draw_context.world.GetComponent<mono_transform_extension::TransformComponent>(
                renderable_entity, mono_transform_extension::TransformComponentHandle::ID());
        assert(transform_component != nullptr && "Renderable entity must have TransformComponent");

        // Create world buffer for geometry pass
        std::unique_ptr<render_graph::geometry_pass::WorldBuffer> world_buffer
            = std::make_unique<render_graph::geometry_pass::WorldBuffer>();
        world_buffer->world_matrix = XMMatrixTranspose(transform_component->GetWorldMatrix());
        world_buffer->world_inverse_transpose
            = XMMatrixTranspose(XMMatrixInverse(nullptr, transform_component->GetWorldMatrix()));

        // Update world buffer in graphics service
        graphics_command_list->UpdateWorldBufferForGeometryPass(
            renderable_component->GetWorldMatrixBufferHandle(), std::move(world_buffer));
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy.SubmitCommandList(std::move(command_list));
}

// Update view-projection matrix from main camera in the scene
void UpdateCamera(RenderContext& draw_context, const render_graph::CommandSetHandle* command_set_handle)
{
    // Get main camera entity
    const ecs::Entity& main_camera_entity = draw_context.scene_component.GetMainCameraEntity();

    // Get CameraComponent
    CameraComponent* camera_component
        = draw_context.world.GetComponent<CameraComponent>(
            main_camera_entity, CameraComponentHandle::ID());
    assert(camera_component != nullptr && "Main camera entity must have CameraComponent");

    // Get transform component
    mono_transform_extension::TransformComponent* transform_component
        = draw_context.world.GetComponent<mono_transform_extension::TransformComponent>(
            main_camera_entity, mono_transform_extension::TransformComponentHandle::ID());
    assert(transform_component != nullptr && "Main camera entity must have TransformComponent");

    // Get trs
    XMFLOAT3 translation = transform_component->GetWorldPosition();
    XMFLOAT4 rotation = transform_component->GetWorldRotation();

    // Calculate view matrix
    XMVECTOR up_vec = XMVectorSet(UP_VECTOR.x, UP_VECTOR.y, UP_VECTOR.z, UP_VECTOR.w);
    XMVECTOR forward_vec = XMVectorSet(FORWARD_VECTOR.x, FORWARD_VECTOR.y, FORWARD_VECTOR.z, FORWARD_VECTOR.w);
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
        = draw_context.graphics_service_proxy.CreateCommandList();
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
    draw_context.graphics_service_proxy.SubmitCommandList(std::move(command_list));
}

void UpdateLight(
    RenderContext& draw_context, 
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle,
    WindowRenderBindComponent* window_render_bind_component,
    uint32_t current_back_buffer_index, uint32_t client_width, uint32_t client_height)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy.CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Iterate through all light entities in the scene
    for (const auto& [component_id, light_entity] : draw_context.scene_component.GetLightEntities())
    {
        // Get LightComponent
        LightComponent* light_component
            = draw_context.world.GetComponent<LightComponent>(light_entity, component_id);
        assert(light_component != nullptr && "Light entity must have LightComponent");

        // Upload light to graphics service
        graphics_command_list->UploadLight(light_component->GetLightHandle());

        // Get transform component
        mono_transform_extension::TransformComponent* transform_component
            = draw_context.world.GetComponent<mono_transform_extension::TransformComponent>(
                light_entity, mono_transform_extension::TransformComponentHandle::ID());
        assert(transform_component != nullptr && "Light entity must have TransformComponent");

        // Update light view-projection matrix
        graphics_command_list->UpdateLightViewProjMatrixBuffer(
            light_component->GetLightHandle(),
            std::make_unique<XMFLOAT3>(transform_component->GetWorldPosition()),
            std::make_unique<XMFLOAT3>(transform_component->GetWorldEulerAngles()));

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
        draw_context.scene_component.GetMaxLightCount());

    {
        // Create shadow composition config buffer data
        std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> shadow_comp_config
            = std::make_unique<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer>();
        shadow_comp_config->screen_size.x = static_cast<float>(client_width);
        shadow_comp_config->screen_size.y = static_cast<float>(client_height);
        shadow_comp_config->shadow_bias = draw_context.scene_component.GetShadowBias();
        shadow_comp_config->slope_scaled_bias = draw_context.scene_component.GetShadowSlopeScaledBias();
        shadow_comp_config->slope_bias_exponent = draw_context.scene_component.GetShadowSlopeBiasExponent();

        // Update shadow composition config buffer in graphics service
        graphics_command_list->UpdateShadowCompositionConfigBuffer(
            window_render_bind_component->GetShadowCompositionConfigBufferHandle(), std::move(shadow_comp_config));
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy.SubmitCommandList(std::move(command_list));
}

// Draw entities in the scene
void DrawEntities(RenderContext& draw_context, 
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy.CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Iterate through all renderable entities in the scene
    for (const ecs::Entity& renderable_entity : draw_context.scene_component.GetRenderableEntities())
    {
        RenderableComponent* renderable_component
            = draw_context.world.GetComponent<RenderableComponent>(
                renderable_entity, RenderableComponentHandle::ID());
        assert(renderable_component != nullptr && "Renderable component is null");

        for (uint32_t i = 0; i < renderable_component->GetIndexCounts()->size(); ++i)
        {
            // Add draw mesh command
            graphics_command_list->DrawMesh(
                renderable_component->GetWorldMatrixBufferHandle(),
                renderable_component->GetMaterialHandles().at(i),
                &renderable_component->GetVertexBufferHandles()->at(i),
                &renderable_component->GetIndexBufferHandles()->at(i),
                renderable_component->GetIndexCounts()->at(i));

            if (renderable_component->NeedsCastShadow())
            {
                // Add draw mesh to shadow map command
                graphics_command_list->DrawShadowCasterMesh(
                    renderable_component->GetWorldMatrixBufferHandle(),
                    &renderable_component->GetVertexBufferHandles()->at(i),
                    &renderable_component->GetIndexBufferHandles()->at(i),
                    renderable_component->GetIndexCounts()->at(i));
            }
        }
    }

    // Submit command list to graphics service
    draw_context.graphics_service_proxy.SubmitCommandList(std::move(command_list));
}

void AddRenderPasses(
    RenderContext& draw_context,
    uint32_t current_back_buffer_index, uint32_t client_width, uint32_t clint_height,
    WindowRenderBindComponent* window_render_bind_component,
    const render_graph::CommandSetHandle* command_set_handle, const render_graph::ResourceHandle* swap_chain_handle,
    render_graph::ImguiPass::DrawFunc ui_draw_func)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = draw_context.graphics_service_proxy.CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Get main camera entity
    const ecs::Entity& main_camera_entity = draw_context.scene_component.GetMainCameraEntity();

    // Get CameraComponent
    CameraComponent* camera_component
        = draw_context.world.GetComponent<CameraComponent>(
            main_camera_entity, CameraComponentHandle::ID());
    assert(camera_component != nullptr && "Main camera entity must have CameraComponent");

    // Get camera transform component
    mono_transform_extension::TransformComponent* camera_transform_component
        = draw_context.world.GetComponent<mono_transform_extension::TransformComponent>(
            main_camera_entity, mono_transform_extension::TransformComponentHandle::ID());
    assert(camera_transform_component != nullptr && "Main camera entity must have TransformComponent");

    // Get scene clear color
    const DirectX::XMFLOAT4& clear_color = draw_context.scene_component.GetClearColor();
    float clear_color_array[4] = {
        clear_color.x, clear_color.y, clear_color.z, clear_color.w };

    // Set command set handle
    graphics_command_list->SetCommandSetHandle(command_set_handle);

    // Set swap chain handle
    graphics_command_list->SetSwapChainHandle(swap_chain_handle);

    // Add light upload pass to graph
    render_graph::Light::LightConfigBuffer light_config;
    light_config.screen_size.x = static_cast<float>(client_width);
    light_config.screen_size.y = static_cast<float>(clint_height);
    light_config.max_lights = draw_context.scene_component.GetMaxLightCount();
    light_config.shadow_intensity = draw_context.scene_component.GetShadowIntensity();
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
        swap_chain_handle, draw_context.scene_component.GetClearColor(),
        &window_render_bind_component->GetLightingRenderTargetTextureHandles()->at(current_back_buffer_index)
        [(uint32_t)render_graph::lighting_pass::RenderTargetIndex::FINAL_COLOR],
        &window_render_bind_component->GetImguiRenderTargetTextureHandles()->at(current_back_buffer_index),
        window_render_bind_component->GetFullScreenTriangleVertexBufferHandle(),
        window_render_bind_component->GetFullScreenTriangleIndexBufferHandle(),
        window_render_bind_component->GetViewPort(), window_render_bind_component->GetScissorRect());

    // Submit command list to graphics service
    draw_context.graphics_service_proxy.SubmitCommandList(std::move(command_list));
}

render_graph::ImguiPass::DrawFunc DrawUI(RenderContext& draw_context)
{
    // List of UI draw functions
    std::vector<render_graph::ImguiPass::DrawFunc> ui_draw_funcs;

    // Iterate through all UI entities in the scene
    for (const ecs::Entity& ui_entity : draw_context.scene_component.GetUIEntities())
    {
        UIComponent* ui_component
            = draw_context.world.GetComponent<UIComponent>(
                ui_entity, UIComponentHandle::ID());
        assert(ui_component != nullptr && "UI component is null");

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
    const ecs::Entity& window_entity = draw_context.scene_component.GetWindowEntity();
    
    // Get WindowComponent
    mono_window_extension::WindowComponent* window_component
        = draw_context.world.GetComponent<mono_window_extension::WindowComponent>(
            window_entity, mono_window_extension::WindowComponentHandle::ID());
    assert(window_component != nullptr && "Window entity must have WindowComponent");

    // Get WindowRenderBindComponent
    WindowRenderBindComponent* window_render_bind_component
        = draw_context.world.GetComponent<WindowRenderBindComponent>(
            window_entity, WindowRenderBindComponentHandle::ID());
    assert(window_render_bind_component != nullptr && "Window entity must have WindowRenderBindComponent");

    // If bind resources are not created, create them and continue
    if (!window_render_bind_component->IsBindResourcesCreated())
    {
        window_render_bind_component->CreateBindResources(
            window_component->GetHwnd(),
            window_component->GetClientWidth(), window_component->GetClientHeight(), 
            BACK_BUFFER_COUNT, draw_context.scene_component.GetMaxLightCount());

        return; // Skip drawing this frame
    }

    // Get current back buffer index
    uint32_t current_back_buffer_index
        = GetCurrentBackBufferIndex(
            draw_context.graphics_service_proxy, window_render_bind_component->GetSwapChainHandle());

    // Get command set handle for current frame
    const render_graph::CommandSetHandle* command_set_handle
        = &window_render_bind_component->GetCommandSetHandles()->at(current_back_buffer_index);

    // Update world matrixes
    UpdateRenderable(draw_context, command_set_handle);

    // Update view-projection matrixes
    UpdateCamera(draw_context, command_set_handle);

    // Update lights
    UpdateLight(
        draw_context, command_set_handle, window_render_bind_component->GetSwapChainHandle(),
        window_render_bind_component, current_back_buffer_index,
        window_component->GetClientWidth(), window_component->GetClientHeight());

    // Draw all renderable entities in the scene
    DrawEntities(draw_context, command_set_handle, window_render_bind_component->GetSwapChainHandle());

    // Draw UI
    render_graph::ImguiPass::DrawFunc ui_draw_func = DrawUI(draw_context);

    // Add render passes to render graph
    AddRenderPasses(
        draw_context, current_back_buffer_index, 
        window_component->GetClientWidth(), window_component->GetClientHeight(),
        window_render_bind_component,
        command_set_handle, window_render_bind_component->GetSwapChainHandle(), std::move(ui_draw_func));
}

GraphicsSystem::GraphicsSystem(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null!");
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool GraphicsSystem::Update(ecs::World& world)
{
    // Iterate through all entities with SceneComponent
    for (const ecs::Entity& entity : world.View(mono_scene_extension::SceneComponentHandle::ID())())
    {
        mono_scene_extension::SceneComponent* scene_component
            = world.GetComponent<mono_scene_extension::SceneComponent>(
                entity, mono_scene_extension::SceneComponentHandle::ID());
        assert(scene_component != nullptr && "SceneComponent is null");

        if (scene_component->GetCurrentState() != mono_scene_extension::SceneState::Playing)
            continue; // Skip if scene is not in Playing state

        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity who has SceneComponent must have MetaComponent");

        if (!meta_component->IsActiveSelf())
            continue; // Skip inactive scenes

        // Create render context
        RenderContext draw_context(
            world, *graphics_service_proxy_, entity, *scene_component);

        // Draw the scene
        DrawScene(draw_context);
    }

    return true; // Success
}

bool GraphicsSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID GraphicsSystem::GetID() const
{
    return GraphicsSystemHandle::ID();
}

} // namespace mono_graphics_extension