#pragma once

#include <functional>
#include <memory>

#include "material_editor/include/dll_config.h"
#include "material_editor/include/setup_param.h"

namespace material_editor
{

// Type alias for setup param edit function
template <typename... Args>
using SetupParamEditFunc 
    = std::function<std::unique_ptr<SetupParamWrapper>(
        const material_editor::SetupParamWrapper*, Args...)>;

// Type alias for material create function
template <typename... Args>
using MaterialCreateFunc = std::function<bool(Args...)>;

} // namespace material_editor