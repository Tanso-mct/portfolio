#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/ui_image.h"
#include "balloon_shooter/include/feature/ui_button.h"

namespace balloon_shooter
{
    const size_t &TitleUIBackgroundComponentID();

    class TitleUIBackgroundComponent : public UIImageComponent
    {
    public:
        TitleUIBackgroundComponent() = default;
        virtual ~TitleUIBackgroundComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &TitleUIPlayBtnComponentID();

    class TitleUIPlayBtnComponent : public UIButtonComponent
    {
    public:
        TitleUIPlayBtnComponent() = default;
        virtual ~TitleUIPlayBtnComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &TitleUIExitBtnComponentID();
    class TitleUIExitBtnComponent : public UIButtonComponent
    {
    public:
        TitleUIExitBtnComponent() = default;
        virtual ~TitleUIExitBtnComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

} // namespace balloon_shooter