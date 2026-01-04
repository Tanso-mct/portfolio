#pragma once

#include "mono_sound/include/dll_config.h"
#include "mono_sound/include/component_audio_listener.h"

namespace mono_sound
{
	class MONO_SOUND_API ComponentAudioSource
	{
	public:
		ComponentAudioSource() = default; // 追加
		ComponentAudioSource(size_t assetID);
		~ComponentAudioSource();

		// Getter
		bool IsInitialized() const { return isInitialized_; }
		bool IsPlaying() const { return isPlaying_; }
		bool IsLooping() const { return isLooping_; }
		float GetVolume() const { return volume_; }
		size_t GetAssetID() const { return assetID_; }

		IXAudio2SourceVoice* GetSourceVoice()  {
			return sourceVoice_;
		}

		// Setter
		void SetInitialized(bool value) { isInitialized_ = value; }
		void SetPlaying(bool value) { isPlaying_ = value; }
		void SetLooping(bool value) { isLooping_ = value; }
		void SetVolume(float value)
		{
			// 範囲をクランプ（0.0f～1.0f）
			if (value < 0.0f) value = 0.0f;
			if (value > 1.0f) value = 1.0f;
			volume_ = value;
		}
		void SetAssetID(size_t value) { assetID_ = value; }

		void SetSourceVoice(IXAudio2SourceVoice* voice) {
			sourceVoice_ = voice;
		}


	private:
		bool isInitialized_ = false;

		bool isPlaying_ = false;
		bool isLooping_ = false;
		float volume_ = 1.0f; // Range from 0.0f to 1.0f
		size_t assetID_;

		IXAudio2SourceVoice* sourceVoice_ = nullptr;
	};

	constexpr size_t ComponentAudioSourceMaxCount = 10000;
	extern MONO_SOUND_API riaecs::ComponentRegistrar<ComponentAudioSource, ComponentAudioSourceMaxCount> ComponentAudioSourceID;

	MONO_SOUND_API void Play(ComponentAudioSource* component);
	MONO_SOUND_API void Stop(ComponentAudioSource* component);
	MONO_SOUND_API void SetLoop(ComponentAudioSource* component, bool isLoop);
	MONO_SOUND_API void SetAudio(ComponentAudioSource* component, size_t assetID);
	//MONO_SOUND_API void Initialize();
	//サウンドのIDはAssetContainerに登録されたAssetのIDを指す
}
