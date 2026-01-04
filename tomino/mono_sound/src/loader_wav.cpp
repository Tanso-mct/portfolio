#include "mono_sound/src/pch.h"
#include "mono_sound/include/loader_wav.h"
#include <mmreg.h>   // WAVEFORMATEX

#include <sstream>
#include <iomanip>   // std::hex, std::setw, std::setfill
#include <string>
#include <Windows.h>


mono_file::WAVFileLoader::WAVFileLoader()
{
}

mono_file::WAVFileLoader::~WAVFileLoader()
{
}

std::unique_ptr<riaecs::IFileData> mono_file::WAVFileLoader::Load(std::string_view filePath) const
{
    std::unique_ptr<uint8_t[]> wavData;
    DirectX::WAVData result;

    HRESULT hr = DirectX::LoadWAVAudioFromFileEx(
        std::wstring(filePath.begin(), filePath.end()).c_str(),
        wavData,
        result
    );

    if (FAILED(hr))
        return nullptr;

    auto soundData = std::make_unique<SoundFileData>();

    // 所有権を移動
    soundData->wavData = std::move(wavData);
    soundData->startAudio = result.startAudio;
    soundData->audioBytes = result.audioBytes;
    soundData->fileName = std::string(filePath);

    // フォーマット情報
    if (result.wfx)
    {
        soundData->format = *result.wfx;

        if (result.wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
        {
            auto ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(result.wfx);
            soundData->extensibleFormat = *ext;  // WAVEFORMATEXTENSIBLEとして保持
            soundData->useExtensible = true;
        }
        else
        {
            soundData->format = *result.wfx;
            soundData->useExtensible = false;
        }
    }

    // デバッグ出力
    OutputDebugStringA(("startAudio ptr: " + std::to_string((uintptr_t)soundData->startAudio) + "\n").c_str());
    OutputDebugStringA(("wavData ptr: " + std::to_string((uintptr_t)soundData->wavData.get()) + "\n").c_str());

    std::ostringstream oss;
    oss << "audioBytes: " << soundData->audioBytes << "\n";
    oss << "first 16 bytes: ";

    for (int i = 0; i < 16 && i < static_cast<int>(soundData->audioBytes); ++i)
    {
        unsigned int byte = static_cast<unsigned int>(soundData->startAudio[i]);
        oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << byte << " ";
    }
    oss << "\n";

    OutputDebugStringA(oss.str().c_str());

    return soundData;
}

MONO_SOUND_API riaecs::FileLoaderRegistrar<mono_file::WAVFileLoader> mono_file::WAVFileLoaderID;

