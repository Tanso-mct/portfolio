#include "render_graph/src/pch.h"
#include "render_graph/include/imgui_context_manager.h"

namespace render_graph
{

ImguiContext& ImguiContextManager::GetContext(const ImguiContextHandle* handle)
{
    assert(handle != nullptr && "ImguiContextHandle pointer is null");
    return context_container_.Get(*handle);
}

bool ImguiContextManager::Contains(const ImguiContextHandle* handle) const
{
    assert(handle != nullptr && "ImguiContextHandle pointer is null");
    return context_container_.Contains(*handle);
}

ImguiContextHandle ImguiContextAdder::Add(std::unique_ptr<ImguiContext> context)
{
    ImguiContextHandle handle = ImguiContextHandle();
    context_container_.WithUniqueLock([&](ImguiContextContainer& container) 
    {
        // Add the context to the container and get its handle
        handle = container.Add(std::move(context));
    });

    return handle; // Return the handle of the added context (or invalid handle on failure)
}

void ImguiContextEraser::Erase(const ImguiContextHandle* handle)
{
    assert(handle != nullptr && "ImguiContextHandle pointer is null");

    context_container_.WithUniqueLock([&](ImguiContextContainer& container) 
    {
        // Erase the context from the container
        container.Erase(*handle);
    });
}


} // namespace render_graph