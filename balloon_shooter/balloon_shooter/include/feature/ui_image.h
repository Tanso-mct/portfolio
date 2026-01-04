#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_image.h"

namespace balloon_shooter
{
    class UIImageComponent : public IUIImageComponent
    {
    public:
        UIImageComponent() = default;
        virtual ~UIImageComponent() = default;
    };

    void ShowUIImage
    (
        wb::IEntity *entity, wb::IComponentContainer &componentCont, wb::IEntityContainer &entityCont,
        bool isShow, size_t componentID
    );

} // namespace balloon_shooter