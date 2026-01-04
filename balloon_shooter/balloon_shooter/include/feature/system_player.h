#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &PlayerSystemID();

    constexpr float GUN_RAY_LENGTH = 10000000.0f; // 銃のレイの長さ、マップ全体どこでも当たるように長く設定
    constexpr float CAMERA_PITCH_LIMIT = 90; // カメラのピッチ制限値

    class PlayerSystem : public wb::ISystem
    {
    private:
        bool isStarted = false;

    public:
        PlayerSystem() = default;
        ~PlayerSystem() override = default;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;

    };

} // namespace balloon_shooter