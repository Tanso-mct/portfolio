#pragma once

#include "ecs/include/entity.h"
#include "mono_graphics_extension/include/ui_drawer.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_proxy_manager.h"
#include "render_graph/include/material.h"
#include "material_editor/include/setup_param.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

constexpr const float MATERIAL_EDITOR_WINDOW_WIDTH_RATIO = 0.4f;
constexpr const float MATERIAL_EDITOR_WINDOW_HEIGHT_RATIO = 0.4f;

constexpr const float MATERIAL_EDITOR_WINDOW_POS_X_RATIO = 0.5f - (MATERIAL_EDITOR_WINDOW_WIDTH_RATIO / 2.0f);
constexpr const float MATERIAL_EDITOR_WINDOW_POS_Y_RATIO = 0.5f - (MATERIAL_EDITOR_WINDOW_HEIGHT_RATIO / 2.0f);

class MONO_ENTITY_ARCHIVE_EXT_DLL MaterialEditorDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    MaterialEditorDrawer(
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy,
        mono_service::ServiceProxyManager& service_proxy_manager);
    ~MaterialEditorDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    struct EditedMaterialInfo
    {
        uint32_t material_handle_key;
        render_graph::MaterialHandle* material_handle = nullptr;
        std::unique_ptr<material_editor::SetupParamWrapper> setup_param = nullptr;
    };

    // Take the edited material setup params
    std::vector<EditedMaterialInfo> TakeEditedMaterialInfos();

    struct MaterialCreateRequest
    {
        bool requested = false;
        render_graph::MaterialTypeHandleID material_type_handle_id = render_graph::MaterialTypeHandleID();
    };

    // Check if create material is requested
    MaterialCreateRequest GetCreateMaterialRequested() const { return create_material_requested_; }

    struct MaterialDeleteRequest
    {
        bool requested = false;
        uint32_t material_handle_key = 0;
    };

    // Check if delete material is requested
    MaterialDeleteRequest GetDeleteMaterialRequested() const { return delete_material_requested_; }

private:
    // The entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_ = nullptr;

    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;

    // The edited material infos
    std::vector<EditedMaterialInfo> edited_material_infos_;

    // Whether the material is selected
    bool selected_ = false;

    // The last selected index
    size_t last_selected_index_ = 0;

    // Whether create material is requested
    MaterialCreateRequest create_material_requested_ = MaterialCreateRequest();

    // Whether delete material is requested
    MaterialDeleteRequest delete_material_requested_ = MaterialDeleteRequest();

    // Index of the material being renamed
    size_t renaming_index_ = 0;

    // Whether renaming is in progress
    bool renaming_ = false;

    // Buffer for renaming input
    char rename_buffer_[256];

    // Flag to request focus on the input box
    bool request_focus_input_ = false;
};

} // namespace mono_entity_archive_extension
