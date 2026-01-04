#include "wbp_locator/src/pch.h"
#include "wbp_locator/include/component_locator.h"

const WBP_LOCATOR_API size_t &wbp_locator::LocatorComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_locator::LocatorComponent::GetID() const
{
    return wbp_locator::LocatorComponentID();
}

void wbp_locator::LocatorComponent::SetLocatorAssetID(const size_t &locatorAssetID)
{
    locatorAssetID_ = locatorAssetID;
    isAssetIDSet_ = true;
}

const size_t &wbp_locator::LocatorComponent::GetLocatorAssetID() const
{
    if (!isAssetIDSet_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Locator asset ID is not set."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_LOCATOR", err);
        wb::ThrowRuntimeError(err);
    }

    return locatorAssetID_;
}

namespace wbp_locator
{
    WB_REGISTER_COMPONENT(LocatorComponentID(), LocatorComponent)

} // namespace wbp_locator