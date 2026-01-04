#include "mono_sound_test/pch.h"

#include "riaecs/include/asset.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_sound/include/asset_sound.h"
#pragma comment(lib, "mono_sound.lib")

#include "mono_sound/include/loader_wav.h"
#include "mono_sound/include/system_sound.h"

#include "mono_sound/include/component_audio_source.h"

#include <filesystem>
#include <iostream>

//TEST(Asset, LoadWavFile)
//{
//    std::filesystem::path wavPath = "../resources/mono_file/400p.wav";
//    std::cout << "Target file: " << wavPath << std::endl;
//
//    ASSERT_TRUE(std::filesystem::exists(wavPath))
//        << "ファイルが存在しません: " << wavPath;
//
//    mono_file::WAVFileLoader wavLoader;
//    auto fileDataBase = wavLoader.Load(wavPath.string());
//
//    ASSERT_NE(fileDataBase, nullptr) << "WAVファイルの読み込みに失敗";
//
//    auto* fileData = dynamic_cast<mono_file::SoundFileData*>(fileDataBase.get());
//    ASSERT_NE(fileData, nullptr) << "SoundFileData へのキャストに失敗";
//
//
//
//    EXPECT_EQ(fileData->fileName, "../resources/mono_file/400p.wav");
//    EXPECT_GE(fileData->format.nSamplesPerSec, 24000);
//    EXPECT_GE(fileData->format.wBitsPerSample, 2);
//    EXPECT_GE(fileData->format.nChannels, 1);
//}

riaecs::AssetSourceRegistrar TestAssetSourceID
(
    "../resources/mono_file/400p.wav",
    mono_file::WAVFileLoaderID(),
    mono_asset::SoundAssetFactoryID()
);


TEST(Asset, LoadWavFile) 
{
    riaecs::ROObject<riaecs::AssetSource> assetSource
        = riaecs::gAssetSourceRegistry->Get(TestAssetSourceID());

    riaecs::ROObject<riaecs::IFileLoader> fileLoader
        = riaecs::gFileLoaderRegistry->Get(assetSource().GetFileLoaderID());

    riaecs::ROObject<riaecs::IAssetFactory> assetFactory
        = riaecs::gAssetFactoryRegistry->Get(assetSource().GetAssetFactoryID());

	// さっき登録したローダーを使ってファイルデータをロード
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load(assetSource().GetFilePath());

    // Prepare the asset staging area using the asset factory
    std::unique_ptr<riaecs::IAssetStagingArea> stagingArea = assetFactory().Prepare();

    // アセットに加工
    std::unique_ptr<riaecs::IAsset> asset = assetFactory().Create(*fileData, *stagingArea);
	auto* soundAsset = dynamic_cast<mono_asset::AssetSound*>(asset.get());

    mono_sound::ComponentAudioSource audioSource(TestAssetSourceID());
    
	mono_sound::SystemSound soundSystem;

    EXPECT_TRUE(soundSystem.PlayAudioSource(nullptr, soundAsset));
    EXPECT_EQ(soundAsset->fileName, "../resources/mono_file/400p.wav");
    EXPECT_GE(soundAsset->format.nSamplesPerSec, 24000);
    EXPECT_GE(soundAsset->format.wBitsPerSample, 2);
    EXPECT_GE(soundAsset->format.nChannels, 1);

}