#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class BalloonShooterWindowFacadeFactory : public wb::IWindowFacadeFactory
    {
    public:
        BalloonShooterWindowFacadeFactory() = default;
        ~BalloonShooterWindowFacadeFactory() override = default;

        std::unique_ptr<wb::IWindowFacade> Create() const override;
    };

} // namespace balloon_shooter