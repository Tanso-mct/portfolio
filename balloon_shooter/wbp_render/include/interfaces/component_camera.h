#pragma once
#include "windows_base/windows_base.h"

namespace wbp_render
{
    class ICameraComponent : public wb::IComponent
    {
    public:
        virtual ~ICameraComponent() = default;

        virtual const float &GetFieldOfView() const = 0;
        virtual void SetFieldOfView(float fov) = 0;

        virtual const float &GetNearZ() const = 0;
        virtual void SetNearZ(float nearZ) = 0;

        virtual const float &GetFarZ() const = 0;
        virtual void SetFarZ(float farZ) = 0;
    };

} // namespace wbp_render