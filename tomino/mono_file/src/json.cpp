#include "mono_file/src/pch.h"
#include "mono_file/include/json.h"

std::unique_ptr<riaecs::IFileData> mono_file::FileLoaderJson::Load(std::string_view filePath) const
{
    std::unique_ptr<FileDataJson> jsonData = std::make_unique<FileDataJson>();

    std::ifstream fileStream(filePath.data());
    if (!fileStream.is_open())
    {
        riaecs::NotifyError({"Failed to open JSON file.", "File path: " + std::string(filePath)}, RIAECS_LOG_LOC);
        return jsonData;
    }

    try
    {
        // Parse JSON content
        nlohmann::json jsonContent;
        fileStream >> jsonContent;

        // Set JSON data
        jsonData->SetJsonData(std::move(jsonContent));
    }
    catch (const nlohmann::json::parse_error& e)
    {
        riaecs::NotifyError(
        {
            "Failed to parse JSON file.",
            "File path: " + std::string(filePath),
            std::string("Error: ") + e.what()
        }, RIAECS_LOG_LOC);
    }

    return jsonData;
}

MONO_FILE_API riaecs::FileLoaderRegistrar<mono_file::FileLoaderJson> mono_file::FileLoaderJsonID;