#include "wbp_locator/src/pch.h"
#include "wbp_locator/include/system_locator.h"

#include "wbp_locator/include/interfaces/asset_locator.h"
#include "wbp_locator/include/component_locator.h"

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

const WBP_LOCATOR_API size_t &wbp_locator::LocatorSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &wbp_locator::LocatorSystem::GetID() const
{
    return wbp_locator::LocatorSystemID();
}

void wbp_locator::LocatorSystem::Initialize(wb::IAssetContainer &assetContainer)
{
}

void wbp_locator::LocatorSystem::Update(const wb::SystemArgument &args)
{
    // Get containers to use
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_locator::LocatorComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *locatorComponent = entity->GetComponent(wbp_locator::LocatorComponentID(), args.componentContainer_);
        wbp_locator::ILocatorComponent *locator = wb::As<wbp_locator::ILocatorComponent>(locatorComponent);

        if (locator->IsLocated())
        {
            // Skip if already located
            continue;
        }

        // Get the locator asset
        wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(locator->GetLocatorAssetID());
        
        // Cast to locator asset interface
        wbp_locator::ILocatorAsset *locatorAsset = wb::As<wbp_locator::ILocatorAsset>(&asset());
        if (locatorAsset == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"The asset which set in the locator component is not a locator asset."}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_LOCATOR", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        if (transform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Locator component requires a TransformComponent to be set"}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_LOCATOR", err);
            wb::ThrowRuntimeError(err);
        }

        // Set the local position from the locator asset
        transform->SetLocalPosition(locatorAsset->GetLocates()[locator->GetLocateTargetIndex()]);
        transform->SetLocalRotation(locatorAsset->GetRotations()[locator->GetLocateTargetIndex()]);

        locator->SetLocated(true);
    }
}

namespace wbp_locator
{
    WB_REGISTER_SYSTEM(LocatorSystem, LocatorSystemID())

} // namespace wbp_locator