#pragma once

#include "ecs/include/entity.h"
#include "mono_graphics_extension/include/ui_drawer.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

class MONO_ENTITY_ARCHIVE_EXT_DLL MenuBarDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    MenuBarDrawer() = default;
    ~MenuBarDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    // Check if quit has been requested
    bool IsQuitRequested() const { return quit_requested_; }

    // Check if showing hierarchy has been requested
    bool IsShowHierarchyRequested() const { return show_hierarchy_requested_; }

    // Check if showing inspector has been requested
    bool IsShowInspectorRequested() const { return show_inspector_requested_; }

    // Check if showing asset browser has been requested
    bool IsShowAssetBrowserRequested() const { return show_asset_browser_requested_; }

    bool IsShowMaterialEditorRequested() const { return show_material_editor_requested_; }

private:
    // Reset all requests
    void ResetRequests();

    // Indicates whether quit has been requested
    bool quit_requested_ = false;

    // Indicates whether showing hierarchy has been requested
    bool show_hierarchy_requested_ = false;

    // Indicates whether showing inspector has been requested
    bool show_inspector_requested_ = false;

    // Indicates whether showing asset browser has been requested
    bool show_asset_browser_requested_ = false;

    // Indicates whether showing material editor has been requested
    bool show_material_editor_requested_ = false;
};

} // namespace mono_entity_archive_extension
