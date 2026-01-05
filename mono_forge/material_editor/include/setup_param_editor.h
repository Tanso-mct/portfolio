#pragma once

#include <functional>
#include <memory>

#include "material_editor/include/json.hpp"
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

// Type alias for setup param export function[]
template <typename ...Args>
using SetupParamExportFunc 
    = std::function<nlohmann::json(const material_editor::SetupParamWrapper*, Args...)>;

// Type alias for setup param import function
template <typename ...Args>
using MaterialImportFunc 
    = std::function<bool(const nlohmann::json& json, Args... args)>;

} // namespace material_editor