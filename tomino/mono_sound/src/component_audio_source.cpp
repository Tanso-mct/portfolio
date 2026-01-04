#include "mono_sound/src/pch.h"
#include "mono_sound/include/component_audio_source.h"

#include "mono_sound/include/system_sound.h"

mono_sound::ComponentAudioSource::ComponentAudioSource(size_t assetID)
{
	assetID_ = assetID;
}

mono_sound::ComponentAudioSource::~ComponentAudioSource()
{
}

MONO_SOUND_API riaecs::ComponentRegistrar
<mono_sound::ComponentAudioSource, mono_sound::ComponentAudioSourceMaxCount> mono_sound::ComponentAudioSourceID;

MONO_SOUND_API void mono_sound::Play(ComponentAudioSource* component)
{
	component->SetPlaying(true);
}

MONO_SOUND_API void mono_sound::Stop(ComponentAudioSource* component)
{
	IXAudio2SourceVoice* sourceVoice = component->GetSourceVoice();
	sourceVoice->Stop(0);
	sourceVoice->FlushSourceBuffers();
	sourceVoice->DestroyVoice();
	sourceVoice = nullptr;
	component->SetPlaying(false);
}

MONO_SOUND_API void mono_sound::SetLoop(ComponentAudioSource* component, bool isLoop)
{
	component->SetLooping(isLoop);
}

MONO_SOUND_API void mono_sound::SetAudio(ComponentAudioSource* component, size_t assetID)
{
	component->SetAssetID(assetID);
}
