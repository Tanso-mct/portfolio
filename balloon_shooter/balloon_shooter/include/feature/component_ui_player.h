#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/ui_image.h"
#include "balloon_shooter/include/feature/ui_bar.h"

namespace balloon_shooter
{
    const size_t &PlayerUIComponentID();

    class PlayerUIComponent : public UIImageComponent
    {
    public:
        PlayerUIComponent() = default;
        virtual ~PlayerUIComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &PlayerUIBalloonLeftComponentID();

    class PlayerUIBalloonLeftComponent : public UIBarComponent
    {
    public:
        PlayerUIBalloonLeftComponent() = default;
        virtual ~PlayerUIBalloonLeftComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

} // namespace balloon_shooter