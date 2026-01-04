#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/asset_browser_ui_component.h"

namespace mono_entity_archive_extension
{

AssetBrowserUIComponent::AssetBrowserUIComponent()
{
}

AssetBrowserUIComponent::~AssetBrowserUIComponent()
{
}

bool AssetBrowserUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    AssetBrowserUIComponent::SetupParam* asset_browser_ui_component_param
        = dynamic_cast<AssetBrowserUIComponent::SetupParam*>(&param);
    assert(asset_browser_ui_component_param != nullptr && "Invalid setup param type for AssetBrowserUIComponent");

    // Set parameters


    return true; // Success
}

bool AssetBrowserUIComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const AssetBrowserUIComponent::SetupParam* asset_browser_ui_component_param
        = dynamic_cast<const AssetBrowserUIComponent::SetupParam*>(&param);
    assert(asset_browser_ui_component_param != nullptr && "Invalid setup param type for AssetBrowserUIComponent");

    // Set parameters

    return true; // Success
}

ecs::ComponentID AssetBrowserUIComponent::GetID() const
{
    return AssetBrowserUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension