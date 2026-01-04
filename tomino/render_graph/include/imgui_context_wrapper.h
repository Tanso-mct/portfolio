#pragma once

#include <functional>
#include <Windows.h>

#include "class_template/instance.h"
#include "imgui.h"
#include "render_graph/include/dll_config.h"

namespace render_graph
{

class RENDER_GRAPH_DLL ImguiContext :
    public class_template::InstanceGuard<
        ImguiContext,
        class_template::ConstructArgList<
            HWND, std::function<ImGuiContext*(HWND)>, std::function<void(ImGuiContext*)>>,
        class_template::SetupArgList<>>
{
public:
    // The type of the context creation function
    using ContextCreateFunc = std::function<ImGuiContext*(HWND)>;

    // The type of the context destruction function
    using ContextDestroyFunc = std::function<void(ImGuiContext*)>;

    ImguiContext(HWND hwnd, ContextCreateFunc create_func, ContextDestroyFunc destroy_func);
    ~ImguiContext();
    bool Setup() override;

    ImGuiContext* Get() const { return context_; }

private:
    // The HWND associated with this ImGui context
    const HWND hwnd_;

    // The function to create the ImGui context
    const ContextCreateFunc create_func_;

    // The function to destroy the ImGui context
    const ContextDestroyFunc destroy_func_;

    // The ImGui context
    ImGuiContext* context_ = nullptr;
};

} // namespace render_graph