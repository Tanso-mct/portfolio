#include "material_editor/src/pch.h"
#include "material_editor/include/setup_param.h"

namespace material_editor
{

SetupParamWrapper::SetupParamWrapper(
    std::unique_ptr<render_graph::Material::SetupParam> setup_param,
    std::unique_ptr<AdditionalSetupParam> additional_param) :
    setup_param_(std::move(setup_param)),
    additional_param_(std::move(additional_param))
{
    assert(setup_param_ != nullptr && "Setup param is null!");
    assert(additional_param_ != nullptr && "Additional setup param is null!");
}

const render_graph::Material::SetupParam* SetupParamWrapper::GetSetupParam() const
{
    return setup_param_.get();
}

const AdditionalSetupParam* SetupParamWrapper::GetAdditionalParam() const
{
    return additional_param_.get();
}

} // namespace material_editor