#pragma once
#include "windows_base/windows_base.h"

namespace example
{
    class GameExampleWindowFacadeFactory : public wb::IWindowFacadeFactory
    {
    public:
        GameExampleWindowFacadeFactory() = default;
        ~GameExampleWindowFacadeFactory() override = default;

        std::unique_ptr<wb::IWindowFacade> Create() const override;
    };

} // namespace example