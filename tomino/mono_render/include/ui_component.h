#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "render_graph/include/imgui_pass.h"
#include "mono_render/include/ui_drawer.h"

namespace mono_render
{
    constexpr size_t UIComponentMaxCount = 100;
    class MONO_RENDER_API UIComponent
    {
    private:
        // The UI drawer
        std::unique_ptr<UIDrawer> ui_drawer_ = nullptr;

    public:
        UIComponent();
        ~UIComponent();

        struct SetupParam
        {
            // The UI drawer
            std::unique_ptr<UIDrawer> ui_drawer = nullptr;
        };
        void Setup(SetupParam &param);

        // Create draw function for ImGui pass by using the UI drawer
        render_graph::ImguiPass::DrawFunc CreateDrawFunc();

        // Get the UI drawer
        UIDrawer& GetDrawer() const;

        // Set the UI drawer
        void SetDrawer(std::unique_ptr<UIDrawer> drawer) { ui_drawer_ = std::move(drawer); }

        // Check if the UI drawer is set
        bool HasDrawer() const { return ui_drawer_ != nullptr; }
    };

    extern MONO_RENDER_API riaecs::ComponentRegistrar
    <UIComponent, UIComponentMaxCount> UIComponentID;

} // namespace mono_render