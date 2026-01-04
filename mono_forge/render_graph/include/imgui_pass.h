#pragma once

#include <vector>

#include "class_template/instance.h"
#include "imgui.h"
#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/imgui_context_handle.h"

namespace render_graph
{

namespace imgui_pass
{

// Clear color for the target texture
const float TARGET_CLEAR_COLOR[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

// Format for the target texture
const DXGI_FORMAT TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;


} // namespace imgui_pass

// Render pass handle for ImguiPass
class RENDER_GRAPH_DLL ImguiPassHandle : 
    public RenderPassHandle<ImguiPassHandle> {};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL ImguiPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        ImguiPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<>>
{
public:
    ImguiPass();
    ~ImguiPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Set the target texture resource handle
    void SetTargetTexture(const ResourceHandle* target_texture_handle);

    // Define the draw function for custom ImGui content
    using DrawFunc = std::function<bool(ImGuiContext*)>;

    // Set the draw function
    void SetDrawFunc(DrawFunc draw_func);

    // Set the ImGui context to use
    void SetImguiContext(const ImguiContextHandle* context_handle);

private:
    // Handle of the target texture resource
    const ResourceHandle* target_texture_handle_ = nullptr;

    // Draw function for custom ImGui content
    DrawFunc draw_func_ = nullptr;

    // ImGui context
    const ImguiContextHandle* context_handle_ = nullptr;
};

} // namespace render_graph