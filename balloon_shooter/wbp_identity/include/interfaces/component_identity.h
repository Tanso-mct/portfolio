#pragma once
#include "windows_base/windows_base.h"

namespace wbp_identity
{
    class IIdentityComponent : public wb::IComponent
    {
    public:
        virtual ~IIdentityComponent() = default;

        virtual std::string_view GetName() = 0;
        virtual void SetName(std::string_view name) = 0;

        virtual const size_t &GetTag() const = 0;
        virtual void SetTag(const size_t &tag) = 0;

        virtual const size_t &GetLayer() const = 0;
        virtual void SetLayer(const size_t &layer) = 0;

        virtual bool IsActiveSelf() const = 0;
        virtual void SetActiveSelf(bool active) = 0;
    };

} // namespace wbp_identity