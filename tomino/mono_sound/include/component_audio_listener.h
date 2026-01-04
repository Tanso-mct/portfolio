#pragma once

//#include "mono_sound/include/dll_config.h"


namespace mono_sound
{
	class MONO_SOUND_API ComponentAudioListener
	{
	public:
		ComponentAudioListener();
		~ComponentAudioListener();

		float GetVolume() const { return volume_; }

		void SetVolume(float value)
		{
			// 範囲をクランプ（0.0f～1.0f）
			if (value < 0.0f) value = 0.0f;
			if (value > 1.0f) value = 1.0f;
			volume_ = value;
		}

	private:
		float volume_ = 1.0f; // Range from 0.0f to 1.0f
	};

	constexpr size_t ComponentAudioListenerMaxCount = 10000;
	extern MONO_SOUND_API riaecs::ComponentRegistrar<ComponentAudioListener, ComponentAudioListenerMaxCount> ComponentAudioListenerID;



}

