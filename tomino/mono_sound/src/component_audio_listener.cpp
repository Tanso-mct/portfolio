#include "mono_sound/src/pch.h"
#include "mono_sound/include/component_audio_listener.h"

mono_sound::ComponentAudioListener::ComponentAudioListener()
{
}

mono_sound::ComponentAudioListener::~ComponentAudioListener()
{
}

MONO_SOUND_API riaecs::ComponentRegistrar
<mono_sound::ComponentAudioListener, mono_sound::ComponentAudioListenerMaxCount> mono_sound::ComponentAudioListenerID;