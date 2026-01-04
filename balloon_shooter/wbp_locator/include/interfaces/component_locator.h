#pragma once
#include "windows_base/windows_base.h"

namespace wbp_locator
{
    class ILocatorComponent : public wb::IComponent
    {
    public:
        virtual ~ILocatorComponent() = default;

        virtual void SetLocatorAssetID(const size_t &locatorAssetID) = 0;
        virtual const size_t &GetLocatorAssetID() const = 0;

        virtual void SetLocateTargetIndex(const size_t &index) = 0;
        virtual const size_t &GetLocateTargetIndex() const = 0;

        virtual bool IsLocated() const = 0;
        virtual void SetLocated(bool located) = 0;
    };

} // namespace wbp_locator