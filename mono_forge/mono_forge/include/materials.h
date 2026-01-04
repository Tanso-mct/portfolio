#pragma once

#include "render_graph/include/material_handle_manager.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

namespace mono_forge
{

// The global material setup param editor registry
extern render_graph::MaterialTypeHandleIDGenerator material_type_handle_id_generator;
extern mono_entity_archive_service::MaterialSetupParamEditorRegistry g_material_setup_param_editor_registry;

// The material handle key for the box lambert material
class BoxLambertMaterialHandleKey :
    public render_graph::MaterialHandleKeyFor<BoxLambertMaterialHandleKey> {};

// The material handle key for the marble bust phong material
class MarbleBustPhongMaterialHandleKey :
    public render_graph::MaterialHandleKeyFor<MarbleBustPhongMaterialHandleKey> {};

// The material handle key for the floor lambert material
class FloorLambertMaterialHandleKey :
    public render_graph::MaterialHandleKeyFor<FloorLambertMaterialHandleKey> {};

} // namespace mono_forge