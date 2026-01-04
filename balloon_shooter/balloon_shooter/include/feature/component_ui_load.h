#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/ui_image.h"

namespace balloon_shooter
{
    const size_t &LoadUIBackgroundComponentID();

    class LoadUIBackgroundComponent : public UIImageComponent
    {
    public:
        LoadUIBackgroundComponent() = default;
        virtual ~LoadUIBackgroundComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

} // namespace balloon_shooter