#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class BalloonShooterWindowEventFactory : public wb::IWindowEventFactory
    {
    public:
        BalloonShooterWindowEventFactory() = default;
        ~BalloonShooterWindowEventFactory() override = default;

        std::unique_ptr<wb::IWindowEvent> Create() const override;
    };

} // namespace balloon_shooter

