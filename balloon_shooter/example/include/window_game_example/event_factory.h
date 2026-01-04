#pragma once
#include "windows_base/windows_base.h"

namespace example
{
    class GameExampleWindowEventFactory : public wb::IWindowEventFactory
    {
    public:
        GameExampleWindowEventFactory() = default;
        ~GameExampleWindowEventFactory() override = default;

        std::unique_ptr<wb::IWindowEvent> Create() const override;
    };

} // namespace example

