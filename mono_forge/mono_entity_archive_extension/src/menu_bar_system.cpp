#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/menu_bar_system.h"

#include "ecs/include/world.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_entity_archive_extension/include/menu_bar_ui_component.h"
#include "mono_entity_archive_extension/include/menu_bar_drawer.h"

namespace mono_entity_archive_extension
{

void ShowUI(ecs::Entity ui_entity, ecs::World& world)
{
    // Get meta component
    mono_meta_extension::MetaComponent* meta_component
        = world.GetComponent<mono_meta_extension::MetaComponent>(
            ui_entity, mono_meta_extension::MetaComponentHandle::ID());
    assert(meta_component != nullptr && "Entity who has MenuBarUIComponent must have MetaComponent");

    // Activate the UI entity
    meta_component->SetActiveSelf(true);

    // Get ui component
    mono_graphics_extension::UIComponent* ui_component
        = world.GetComponent<mono_graphics_extension::UIComponent>(
            ui_entity, mono_graphics_extension::UIComponentHandle::ID());
    assert(ui_component != nullptr && "Entity who has MenuBarUIComponent must have UIComponent");

    // Show the UI window
    ui_component->GetDrawer().SetShowWindow(true);
}

MenuBarSystem::MenuBarSystem()
{
}

MenuBarSystem::~MenuBarSystem()
{
}

bool MenuBarSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool MenuBarSystem::Update(ecs::World& world)
{
    // Iterate through all entities with MenuBarUIComponent
    for (const ecs::Entity& entity : world.View(MenuBarUIComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity who has MenuBarUIComponent must have MetaComponent");

        if (!meta_component->IsActiveSelf())
            continue; // Skip inactive menu bars

        // Get menu bar ui component
        MenuBarUIComponent* menu_bar_ui_component
            = world.GetComponent<MenuBarUIComponent>(
                entity, MenuBarUIComponentHandle::ID());
        assert(menu_bar_ui_component != nullptr && "Entity who has MenuBarUIComponent must have MenuBarUIComponent");

        // Get ui component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "Entity who has MenuBarUIComponent must have UIComponent");

        // Get ui drawer
        MenuBarDrawer* menu_bar_drawer = dynamic_cast<MenuBarDrawer*>(&ui_component->GetDrawer());
        assert(menu_bar_drawer != nullptr && "UIComponent drawer must be MenuBarDrawer");

        if (menu_bar_drawer->IsQuitRequested())
            return false; // Request to quit application

        if (menu_bar_drawer->IsShowHierarchyRequested())
            ShowUI(menu_bar_ui_component->GetHierarchyEntity(), world);

        if (menu_bar_drawer->IsShowInspectorRequested())
            ShowUI(menu_bar_ui_component->GetInspectorEntity(), world);

        if (menu_bar_drawer->IsShowAssetBrowserRequested())
            ShowUI(menu_bar_ui_component->GetAssetBrowserEntity(), world);

        if (menu_bar_drawer->IsShowMaterialEditorRequested())
            ShowUI(menu_bar_ui_component->GetMaterialEditorEntity(), world);
    }

    return true; // Success
}

bool MenuBarSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID MenuBarSystem::GetID() const
{
    return MenuBarSystemHandle::ID();
}

} // namespace mono_entity_archive_extension