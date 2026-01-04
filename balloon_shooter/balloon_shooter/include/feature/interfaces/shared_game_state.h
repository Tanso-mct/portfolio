#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class IGameStateSharedFacade : public wb::ISharedFacade
    {
    public:
        virtual ~IGameStateSharedFacade() = default;

        virtual void SetContext(std::unique_ptr<wb::IContext> context) override {}
        virtual bool CheckIsReady() const override { return true; }

        virtual const size_t &GetGameState() const = 0;
        virtual void SetGameState(size_t state) = 0;

        virtual bool IsSwitched(size_t &beforeState) const = 0;
        virtual void ResetSwitchedFlag() = 0;

        virtual bool IsFirstStateInitialized() = 0;
        virtual void SetFirstStateInitialized(bool isFirst) = 0;

        virtual bool IsSceneSwitching() const = 0;
        virtual void SetIsSceneSwitching(bool isSwitching) = 0;
    };

} // namespace balloon_shooter