#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/ui_image.h"
#include "balloon_shooter/include/feature/ui_button.h"

namespace balloon_shooter
{
    const size_t &ClearUIBackgroundComponentID();

    class ClearUIBackgroundComponent : public UIImageComponent
    {
    public:
        ClearUIBackgroundComponent() = default;
        virtual ~ClearUIBackgroundComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &ClearUIPlayBtnComponentID();

    class ClearUIPlayBtnComponent : public UIButtonComponent
    {
    public:
        ClearUIPlayBtnComponent() = default;
        virtual ~ClearUIPlayBtnComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &ClearUIMenuBtnComponentID();

    class ClearUIMenuBtnComponent : public UIButtonComponent
    {
    public:
        ClearUIMenuBtnComponent() = default;
        virtual ~ClearUIMenuBtnComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

} // namespace balloon_shooter