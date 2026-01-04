#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/renderable_component.h"

#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace mono_graphics_extension
{

RenderableComponent::RenderableComponent(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null");
}

RenderableComponent::~RenderableComponent()
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create GraphicsCommandList");

    // Destroy world matrix buffer
    graphics_command_list->DestroyResource(&world_matrix_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

bool RenderableComponent::Setup(ecs::Component::SetupParam& param)
{
    // Try to cast setup param to RenderableComponent::SetupParam
    RenderableComponent::SetupParam* renderable_param
        = dynamic_cast<RenderableComponent::SetupParam*>(&param);
    assert(renderable_param != nullptr && "Invalid setup param type for RenderableComponent");

    assert(!renderable_param->mesh_set_.vertex_buffer_handles.empty() && "Vertex buffer handles cannot be empty");
    for (int i = 0; i < renderable_param->mesh_set_.vertex_buffer_handles.size(); ++i)
        assert(renderable_param->mesh_set_.vertex_buffer_handles[i].IsValid() && "Invalid vertex buffer handle in setup param");

    assert(!renderable_param->mesh_set_.index_buffer_handles.empty() && "Index buffer handles cannot be empty");
    for (int i = 0; i < renderable_param->mesh_set_.index_buffer_handles.size(); ++i)
        assert(renderable_param->mesh_set_.index_buffer_handles[i].IsValid() && "Invalid index buffer handle in setup param");

    assert(!renderable_param->mesh_set_.index_counts.empty() && "Index counts cannot be empty");
    for (const auto& count : renderable_param->mesh_set_.index_counts)
        assert(count > 0 && "Invalid index count in setup param");

    assert(!renderable_param->mesh_set_.material_handles.empty() && "Material handles cannot be empty");
    for (const auto* handle : renderable_param->mesh_set_.material_handles)
        assert(handle != nullptr && "Invalid material handle in setup param");

    // Store setup param values
    vertex_buffer_handles_ = renderable_param->mesh_set_.vertex_buffer_handles;
    index_buffer_handles_ = renderable_param->mesh_set_.index_buffer_handles;
    index_counts_ = renderable_param->mesh_set_.index_counts;
    material_handles_ = renderable_param->mesh_set_.material_handles;
    cast_shadow_ = renderable_param->cast_shadow_;

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create GraphicsCommandList");

    // Create world matrix buffer
    graphics_command_list->CreateMatrixBuffer(&world_matrix_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));

    return true; // Success
}

bool RenderableComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Try to cast setup param to RenderableComponent::SetupParam
    const RenderableComponent::SetupParam* renderable_param
        = dynamic_cast<const RenderableComponent::SetupParam*>(&param);
    assert(renderable_param != nullptr && "Invalid setup param type for RenderableComponent");

    // Update setup param values
    vertex_buffer_handles_ = renderable_param->mesh_set_.vertex_buffer_handles;
    index_buffer_handles_ = renderable_param->mesh_set_.index_buffer_handles;
    index_counts_ = renderable_param->mesh_set_.index_counts;
    material_handles_ = renderable_param->mesh_set_.material_handles;
    cast_shadow_ = renderable_param->cast_shadow_;

    return true; // Success
}

ecs::ComponentID RenderableComponent::GetID() const
{
    return RenderableComponentHandle::ID();
}

} // namespace mono_graphics_extension