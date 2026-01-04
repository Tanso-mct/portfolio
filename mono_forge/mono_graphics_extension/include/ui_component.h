#pragma once

#include "ecs/include/component.h"
#include "render_graph/include/imgui_pass.h"
#include "mono_graphics_extension/include/dll_config.h"
#include "mono_graphics_extension/include/ui_drawer.h"

namespace mono_graphics_extension
{

// The handle class for the component
class MONO_GRAPHICS_EXT_DLL UIComponentHandle : public ecs::ComponentHandle<UIComponentHandle> {};

// The component class
class MONO_GRAPHICS_EXT_DLL UIComponent :
    public ecs::Component
{
public:
    UIComponent();
    virtual ~UIComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            ui_drawer(std::make_unique<DefaultUIDrawer>())
        {
        }

        virtual ~SetupParam() override = default;

        // The UI drawer
        std::unique_ptr<UIDrawer> ui_drawer;

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Create draw function for ImGui pass by using the UI drawer
    render_graph::ImguiPass::DrawFunc CreateDrawFunc();

    // Get the UI drawer
    UIDrawer& GetDrawer() const;

    // Set the UI drawer
    void SetDrawer(std::unique_ptr<UIDrawer> drawer) { ui_drawer_ = std::move(drawer); }

    // Check if the UI drawer is set
    bool HasDrawer() const { return ui_drawer_ != nullptr; }

private:
    // The UI drawer
    std::unique_ptr<UIDrawer> ui_drawer_ = nullptr;

};

} // namespace mono_graphics_extension