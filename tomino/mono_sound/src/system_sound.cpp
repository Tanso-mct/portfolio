#include "mono_sound/src/pch.h"
#include "mono_sound/include/system_sound.h"

bool mono_sound::SystemSound::PlayAudioSource(mono_sound::ComponentAudioSource* audioSource, const mono_asset::AssetSound* soundAsset)
{
    IXAudio2SourceVoice* pSourceVoice = audioSource->GetSourceVoice();
    if (!pSourceVoice)
    {
        const WAVEFORMATEX* format =
            soundAsset->useExtensible ? reinterpret_cast<const WAVEFORMATEX*>(&soundAsset->extensibleFormat) :
            &soundAsset->format;

        HRESULT hr = xAudio2_->CreateSourceVoice(&pSourceVoice, format);
        if (FAILED(hr))
        {
            riaecs::NotifyError({ "Failed to create XAudio2 SourceVoice" }, RIAECS_LOG_LOC);
            return false;
        }

        audioSource->SetSourceVoice(pSourceVoice);

        XAUDIO2_BUFFER buffer = {};
        buffer.pAudioData = soundAsset->pcmData.data();
        buffer.AudioBytes = static_cast<UINT32>(soundAsset->pcmData.size());
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.LoopCount = audioSource->IsLooping() ? XAUDIO2_LOOP_INFINITE : 0;

        hr = pSourceVoice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
        {
            riaecs::NotifyError({ "Failed to submit XAudio2 buffer" }, RIAECS_LOG_LOC);
            pSourceVoice->DestroyVoice();
            audioSource->SetSourceVoice(nullptr);
            return false;
        }

        hr = pSourceVoice->Start(0);
        if (FAILED(hr))
        {
            riaecs::NotifyError({ "Failed to start XAudio2 voice" }, RIAECS_LOG_LOC);
            pSourceVoice->DestroyVoice();
            audioSource->SetSourceVoice(nullptr);
            return false;
        }
    }
    else
    {
        // 再生中なら何もしない（ループ再生の場合は追加のバッファ不要）
    }

    return true;
}

MONO_SOUND_API mono_sound::SystemSound::SystemSound()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        riaecs::NotifyError({ "Failed to initialize COM library" }, RIAECS_LOG_LOC);
        return;
	}
    hr = XAudio2Create(xAudio2_.GetAddressOf(), 0);
    if (FAILED(hr)) {
        riaecs::NotifyError({ "Failed to initialize XAudio2" }, RIAECS_LOG_LOC);
        return;
    }
    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
    if (FAILED(hr)) {
        riaecs::NotifyError({ "Failed to create XAudio2 Mastering Voice" }, RIAECS_LOG_LOC);
        return;
	}
}

MONO_SOUND_API mono_sound::SystemSound::~SystemSound()
{
    if (masterVoice_) {
        masterVoice_->DestroyVoice();
        masterVoice_ = nullptr;
    }
    if (xAudio2_) {
        xAudio2_->Release();
        xAudio2_ = nullptr;
	}
}

MONO_SOUND_API bool mono_sound::SystemSound::Update(
    riaecs::IECSWorld& ecsWorld,
    riaecs::IAssetContainer& assetCont,
    riaecs::ISystemLoopCommandQueue& systemLoopCmdQueue)
{
    
  //  for (const riaecs::Entity& entity : ecsWorld.View(mono_sound::ComponentAudioListenerID())())
  //  {
  //      mono_sound::ComponentAudioListener* listenerEntities
  //          = riaecs::GetComponentWithCheck<mono_sound::ComponentAudioListener>(
  //              ecsWorld, entity, mono_sound::ComponentAudioListenerID(), "ComponentAudioListener", RIAECS_LOG_LOC);
		//if (!listenerEntities) continue;
  //  }

    for (const riaecs::Entity& entity : ecsWorld.View(mono_sound::ComponentAudioSourceID())())
    {
        mono_sound::ComponentAudioSource* audioSource
            = riaecs::GetComponentWithCheck<mono_sound::ComponentAudioSource>(
                ecsWorld, entity, mono_sound::ComponentAudioSourceID(), "ComponentAudioSource", RIAECS_LOG_LOC);

		if (!audioSource) continue;

        //componentのisPlayingがtrueなら再生する

        if (audioSource->IsPlaying())
        {
            riaecs::ID assetID(audioSource->GetAssetID(), assetCont.GetGeneration(audioSource->GetAssetID()));
            riaecs::ROObject<riaecs::IAsset> asset = assetCont.Get(assetID);
            const riaecs::IAsset& baseAsset = asset();
            const mono_asset::AssetSound* soundAsset = dynamic_cast<const mono_asset::AssetSound*>(&baseAsset);

            if (!soundAsset)
            {
                riaecs::NotifyError({ "AssetSound not found or type mismatch" }, RIAECS_LOG_LOC);
                continue;
            }

            // 実際の再生処理は PlayAudioSource に任せる
            PlayAudioSource(audioSource, soundAsset);
        }
        else
        {
            // 停止時は Stop するだけ（必要に応じて）
            if (auto* voice = audioSource->GetSourceVoice())
            {
                voice->Stop(0);
                voice->FlushSourceBuffers();
            }
        }

    }

    return true;
}
