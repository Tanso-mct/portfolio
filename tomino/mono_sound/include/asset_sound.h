#pragma once

#include "riaecs/include/interfaces/asset.h"
#include "mono_sound/include/dll_config.h"
#include "riaecs/include/global_registry.h"

#include <iostream>
#include <cstdint>     // uint8_t 使うなら必要

#include <xaudio2.h>
#include <xaudio2fx.h>



/*******WAVEファイル読み込み用データ先頭文字列定義
バイナリデータのヘッダを取得する際に使う文字列。データの先頭にこれが入っている。
この先頭文字列から始まるデータの塊をチャンクと呼ぶ。
だけども、WindowsのX86,X64互換CPUはリトルエンディアンなのでバイナリを直接読み込むと読み込んだデータが8bit順単位で
ひっくり返るので文字列を逆に設定する。
XBOX360とPS3の次代はビッグエンディアン（ひっくり返らない）だったのでビッグエンディアン用とリトル用を用意したが、
XBOX ONE、PS4次代からCPUがリトルエンディアンになった。と言うかもうほとんどWindows。
*******/

namespace mono_asset {

    class MONO_SOUND_API AssetSound : // SoundFileDataから必要な情報だけとってくる
        public riaecs::IAsset
    {
    public:
        AssetSound();
        ~AssetSound() override;

        std::string fileName;

        std::vector<uint8_t> pcmData;//TODO！！！ポインタにする

		bool useExtensible = false; // WAVEFORMATEXTENSIBLEを使うかどうか

		WAVEFORMATEXTENSIBLE extensibleFormat{}; // 拡張用
        WAVEFORMATEX format = {};
        int durationInSeconds = 0;
    };

    class MONO_SOUND_API SoundAssetStagingArea : public riaecs::IAssetStagingArea {
    public:
        // サウンド特有の一時的データを持たせたいならここにいれるらしい
    };


    class MONO_SOUND_API SoundAssetFactory : public riaecs::IAssetFactory 
    {
    public:
        SoundAssetFactory();

        ~SoundAssetFactory() override;

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override;

        std::unique_ptr<riaecs::IAsset> Create(
            const riaecs::IFileData& fileData,
            riaecs::IAssetStagingArea& stagingArea
        ) const override;

        void Commit(riaecs::IAssetStagingArea& stagingArea) const override {};

    };

    extern MONO_SOUND_API riaecs::AssetFactoryRegistrar<SoundAssetFactory> SoundAssetFactoryID;



}