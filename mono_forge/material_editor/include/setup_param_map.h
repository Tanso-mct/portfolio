#pragma once

#include <unordered_map>
#include <memory>

#include "render_graph/include/material_handle.h"
#include "material_editor/include/setup_param.h"

namespace material_editor
{

// The type alias for mapping material handles to their setup parameters
using SetupParamMap = std::unordered_map<render_graph::MaterialHandle, std::unique_ptr<SetupParamWrapper>>;

} // namespace material_editor