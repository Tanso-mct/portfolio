#pragma once
#include "mono_sound/include/WAVFileReader.h"
#include <riaecs/include/global_registry.h>



#include <xaudio2.h>
#include <xaudio2fx.h>

namespace mono_file {

    class MONO_SOUND_API SoundFileData : public riaecs::IFileData {
    public:
		std::unique_ptr<uint8_t[]> wavData; // バッキングデータ
		uint32_t audioBytes = 0; // PCMデータのバイト数
        const uint8_t* startAudio = nullptr; // PCMデータの先頭ポインタ
        uint32_t channelMask = 0; // チャンネルマスク
		std::string fileName; // ファイル名

        WAVEFORMATEX format = {};                // PCM 用（既存）
        WAVEFORMATEXTENSIBLE extensibleFormat{}; // 拡張用
        bool useExtensible = false;              // どちらを使うか

    };

    class MONO_SOUND_API WAVFileLoader : public riaecs::IFileLoader
    {
    public:
        WAVFileLoader();

        ~WAVFileLoader() override;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };

    extern MONO_SOUND_API riaecs::FileLoaderRegistrar<mono_file::WAVFileLoader> WAVFileLoaderID;

    //extern riaecs::FileLoaderRegistrar<WAVFileLoader> WAVFileLoaderID;
    //cppの変数がheaderを通じて公開されるらしい


}
