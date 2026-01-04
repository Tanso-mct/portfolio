#pragma once
#include "mono_file/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_file/include/json.hpp"

namespace mono_file
{
    // JSON file data
    class MONO_FILE_API FileDataJson : public riaecs::IFileData
    {
    private:
        nlohmann::json jsonData_;

    public:
        FileDataJson() = default;
        ~FileDataJson() override = default;

        const nlohmann::json& GetJsonData() const { return jsonData_; }
        void SetJsonData(nlohmann::json&& jsonData) { jsonData_ = std::move(jsonData); }
    };

    // JSON file loader
    class MONO_FILE_API FileLoaderJson : public riaecs::IFileLoader
    {
    public:
        FileLoaderJson() = default;
        ~FileLoaderJson() override = default;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };
    extern MONO_FILE_API riaecs::FileLoaderRegistrar<FileLoaderJson> FileLoaderJsonID;
    
} // namespace mono_file