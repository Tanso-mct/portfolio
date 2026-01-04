#include "render_graph/src/pch.h"
#include "render_graph/include/imgui_context_wrapper.h"

namespace render_graph
{

ImguiContext::ImguiContext(HWND hwnd, ContextCreateFunc create_func, ContextDestroyFunc destroy_func) :
    hwnd_(hwnd),
    create_func_(std::move(create_func)),
    destroy_func_(std::move(destroy_func))
{
}

ImguiContext::~ImguiContext()
{
    // Destroy ImGui context using the provided destruction function
    if (context_ != nullptr)
    {
        destroy_func_(context_);
        context_ = nullptr;
    }
}

bool ImguiContext::Setup()
{
    // Create ImGui context using the provided creation function
    context_ = create_func_(hwnd_);
    if (context_ == nullptr)
        return false; // Failed to create context

    return true;
}

} // namespace render_graph