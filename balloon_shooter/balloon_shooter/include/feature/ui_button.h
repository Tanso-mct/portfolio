#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_button.h"

namespace balloon_shooter
{
    class UIButtonComponent : public IUIButtonComponent
    {
    private:
        std::unique_ptr<IButtonEvent> event_ = nullptr;
        bool isHovering_ = false;

    public:
        UIButtonComponent() = default;
        virtual ~UIButtonComponent() override = default;

        UIButtonComponent(const UIButtonComponent &) = delete;
        UIButtonComponent &operator=(const UIButtonComponent &) = delete;

        /***************************************************************************************************************
         * IUIButtonComponent implementation
        /**************************************************************************************************************/

        void SetEvent(std::unique_ptr<IButtonEvent> event) override { event_ = std::move(event); }
        IButtonEvent &GetEvent() override;

        void SetIsHovering(bool isHovering) override { isHovering_ = isHovering; }
        bool IsHovering() const override { return isHovering_; }
        
    };

    void ShowUIButton
    (
        wb::IEntity *entity, wb::IComponentContainer &componentCont, wb::IEntityContainer &entityCont,
        bool isShow, size_t componentID
    );

} // namespace balloon_shooter