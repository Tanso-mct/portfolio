#pragma once

#include <functional>
#include <Windows.h>

#include "class_template/singleton.h"
#include "utility_header/container.h"
#include "imgui/include/imgui.h"
#include "render_graph/include/dll_config.h"
#include "render_graph/include/imgui_context_handle.h"
#include "render_graph/include/imgui_context_wrapper.h"

namespace render_graph
{

// The container type used to contain ImGui contexts
using ImguiContextContainer = utility_header::Container<ImguiContext>;

// The manager class that manages ImGui contexts and their handles
class RENDER_GRAPH_DLL ImguiContextManager :
    public class_template::Singleton<ImguiContextManager>
{
public:
    ImguiContextManager(ImguiContextContainer& context_container) :
        context_container_(context_container) 
    {
    }

    ~ImguiContextManager() override = default;

    // Execute a function with a unique lock on the ImGui context container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        context_container_.WithUniqueLock([&](ImguiContextContainer& container) 
        {
            func(*this);
        });
    }

    // Get the ImguiContext associated with the given handle
    // It must be called in WithLock lambda function
    ImguiContext& GetContext(const ImguiContextHandle* handle);

    // Check if the Imgui context container contains the given handle
    bool Contains(const ImguiContextHandle* handle) const;

private:
    // The container that holds ImGui contexts
    ImguiContextContainer& context_container_;
};

// The adder class that adds ImGui contexts to the context_container
class RENDER_GRAPH_DLL ImguiContextAdder
{
public:
    ImguiContextAdder(ImguiContextContainer& context_container) : context_container_(context_container) {}
    ~ImguiContextAdder() = default;

    // Add an ImGui context and return its handle
    ImguiContextHandle Add(std::unique_ptr<ImguiContext> context);

private:
    // The container that holds ImGui contexts
    ImguiContextContainer& context_container_;
};

// The eraser class that removes ImGui contexts from the context_container
class RENDER_GRAPH_DLL ImguiContextEraser
{
public:
    ImguiContextEraser(ImguiContextContainer& context_container) : context_container_(context_container) {}
    ~ImguiContextEraser() = default;

    // Erase an ImGui context using its handle
    void Erase(const ImguiContextHandle* handle);

private:
    // The container that holds ImGui contexts
    ImguiContextContainer& context_container_;
};

} // namespace render_graph