#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/game_state.h"

namespace balloon_shooter
{
    const size_t &UIControllerSystemID();

    class UIControllerSystem : public wb::ISystem
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<balloon_shooter::IGameState>> gameStates_;
        
    public:
        UIControllerSystem() = default;
        ~UIControllerSystem() override = default;

        UIControllerSystem(const UIControllerSystem &) = delete;
        UIControllerSystem &operator=(const UIControllerSystem &) = delete;

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;
    };

} // namespace balloon_shooter