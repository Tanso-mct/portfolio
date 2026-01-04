#pragma once

#include "material_editor/include/dll_config.h"
#include "render_graph/include/material.h"

namespace material_editor
{

// An additional setup param interface for extension
class AdditionalSetupParam
{
public:
    virtual ~AdditionalSetupParam() = default;

    // Clone the additional setup param
    virtual std::unique_ptr<AdditionalSetupParam> Clone() const { return nullptr; }
};

// The setup param wrapper class
class MATERIAL_EDITOR_DLL SetupParamWrapper
{
public:
    SetupParamWrapper(
        std::unique_ptr<render_graph::Material::SetupParam> setup_param,
        std::unique_ptr<AdditionalSetupParam> additional_param) ;
    ~SetupParamWrapper() = default;

    // Get the setup param
    const render_graph::Material::SetupParam* GetSetupParam() const;

    // Get the additional setup param
    const AdditionalSetupParam* GetAdditionalParam() const;

private:
    // Underlying setup param
    std::unique_ptr<render_graph::Material::SetupParam> setup_param_ = nullptr;

    // Additional setup param
    std::unique_ptr<AdditionalSetupParam> additional_param_ = nullptr;
};

} // namespace material_editor