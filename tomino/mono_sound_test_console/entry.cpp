#pragma once

#include <iostream>

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")


#include "mono_sound/include/system_sound.h"
#include "mono_sound/include/asset_sound.h"
#include "mono_sound/include/loader_wav.h"
#pragma comment(lib, "mono_sound.lib")

namespace mono_sound_test_console 
{

riaecs::AssetSourceRegistrar SoundAssetSourceID(
    "../resources/mono_file/400p.wav", mono_file::WAVFileLoaderID(), mono_asset::SoundAssetFactoryID());


} // namespace mono_sound_test_console

void CheckVoiceState(mono_sound::ComponentAudioSource* audioSource)
{
    IXAudio2SourceVoice* voice = audioSource->GetSourceVoice();
    if (!voice) return;

    XAUDIO2_VOICE_STATE state = {};
    audioSource->GetSourceVoice()->GetState(&state);

    std::cout << "BuffersQueued: " << state.BuffersQueued
        << ", SamplesPlayed: " << state.SamplesPlayed
        << std::endl;
}

int main()
{
    // Asset Sourceを取得
    riaecs::ROObject<riaecs::AssetSource> assetSource 
        = riaecs::gAssetSourceRegistry->Get(mono_sound_test_console::SoundAssetSourceID());

    // Consoleに表示
    std::cout << "Asset Source File Path: " << assetSource().GetFilePath() << std::endl;
    std::cout << "File Loader ID: " << assetSource().GetFileLoaderID() << std::endl;
    std::cout << "Asset Factory ID: " << assetSource().GetAssetFactoryID() << std::endl;

    // File Loaderを取得
    riaecs::ROObject<riaecs::IFileLoader> fileLoader 
        = riaecs::gFileLoaderRegistry->Get(assetSource().GetFileLoaderID());

    // Asset Factoryを取得
    riaecs::ROObject<riaecs::IAssetFactory> assetFactory 
        = riaecs::gAssetFactoryRegistry->Get(assetSource().GetAssetFactoryID());

    // File Loaderを使ってファイルデータをロード
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load(assetSource().GetFilePath());
    if (!fileData) // ロード失敗時のエラーハンドリング
    {
        std::cerr << "Failed to load file data from: " << assetSource().GetFilePath() << std::endl;
        return -1;
    }

    // Asset Factoryを使ってアセットステージングエリアを準備
    std::unique_ptr<riaecs::IAssetStagingArea> stagingArea = assetFactory().Prepare();
    if (!stagingArea) // 準備失敗時のエラーハンドリング
    {
        std::cerr << "Failed to prepare asset staging area." << std::endl;
        return -1;
    }

    // ファイルデータを使ってアセットを作成
    std::unique_ptr<riaecs::IAsset> asset = assetFactory().Create(*fileData, *stagingArea);
    if (!asset) // 作成失敗時のエラーハンドリング
    {
        std::cerr << "Failed to create asset from file data." << std::endl;
        return -1;
    }

    // アセットステージングエリアをコミット
    assetFactory().Commit(*stagingArea);

    // 成功メッセージを表示
    std::cout << "Asset creation process completed successfully." << std::endl;

    mono_asset::AssetSound*  soundasset = dynamic_cast<mono_asset::AssetSound*>(asset.get());

    mono_sound::SystemSound soundSystem;
	mono_sound::ComponentAudioSource audioSource(mono_sound_test_console::SoundAssetSourceID());

    

    // メインループやタイマーで audioSource が生きている間、再生を確認
    while (true)
    {
        soundSystem.PlayAudioSource(&audioSource, soundasset);
    }

    return 0;
}