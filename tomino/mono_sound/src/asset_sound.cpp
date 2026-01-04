#include "mono_sound/src/pch.h"
#include "mono_sound/include/asset_sound.h"
#include "mono_sound/include/loader_wav.h"


mono_asset::SoundAssetFactory::SoundAssetFactory()
{
}

mono_asset::SoundAssetFactory::~SoundAssetFactory()
{
}

std::unique_ptr<riaecs::IAssetStagingArea> mono_asset::SoundAssetFactory::Prepare() const
{
    return std::make_unique<SoundAssetStagingArea>();
}

MONO_SOUND_API riaecs::AssetFactoryRegistrar <mono_asset::SoundAssetFactory> mono_asset::SoundAssetFactoryID;


std::unique_ptr<riaecs::IAsset> mono_asset::SoundAssetFactory::Create(const riaecs::IFileData& fileData, riaecs::IAssetStagingArea& stagingArea) const
{
    // fileData を SoundFileData にキャスト。他のやつもやるのかな？mp3とか入れる場合は。今回はやらんと思うけど
    const mono_file::SoundFileData& soundData = dynamic_cast<const mono_file::SoundFileData&>(fileData);

    // AssetSound を生成
    auto asset = std::make_unique<mono_asset::AssetSound>();
    asset->fileName = soundData.fileName;
	asset->format = soundData.format;//Todo なんか、これを使うのかいなか考えてください
	asset->format.cbSize = 0; // PCMなら常に0
    asset->pcmData.resize(soundData.audioBytes);
    std::memcpy(asset->pcmData.data(), soundData.startAudio, soundData.audioBytes);
	asset->durationInSeconds = soundData.audioBytes / (soundData.format.nAvgBytesPerSec);

	asset->useExtensible = soundData.useExtensible;
	if (soundData.useExtensible) {
		asset->extensibleFormat = soundData.extensibleFormat;
	}

    return asset;
}

mono_asset::AssetSound::AssetSound()
{
}

mono_asset::AssetSound::~AssetSound()
{
}
