#pragma once
#include "riaecs/include/interfaces/ecs.h"
#include "mono_sound/include/asset_sound.h"
#include "mono_sound/include/component_audio_source.h"
#include <wrl/client.h>

namespace mono_sound {

    class MONO_SOUND_API SystemSound :
        public riaecs::ISystem
    {
    private:
        Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
        IXAudio2MasteringVoice* masterVoice_;

    public:
        SystemSound();
        ~SystemSound();
        bool Update
        (
            riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont,
            riaecs::ISystemLoopCommandQueue& systemLoopCmdQueue
        )override;

        bool PlayAudioSource(mono_sound::ComponentAudioSource* audioSource, const mono_asset::AssetSound* soundAsset);
    };
}

