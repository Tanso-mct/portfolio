#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class IGameState
    {
    public:
        virtual ~IGameState() = default;
        virtual const size_t &GetID() const = 0;

        virtual void UIUpdate(const wb::SystemArgument &args) = 0;
        virtual void Switched(const wb::SystemArgument &args) = 0;
    };

} // namespace balloon_shooter