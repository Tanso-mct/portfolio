#include "mono_file_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_file/include/json.h"
#pragma comment(lib, "mono_file.lib")

TEST(Json, Load)
{
    riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderJsonID());
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load("../resources/mono_file_test/json/sample.json");
    ASSERT_NE(fileData, nullptr);

    mono_file::FileDataJson &jsonData = static_cast<mono_file::FileDataJson&>(*fileData);
    const nlohmann::json &jsonContent = jsonData.GetJsonData();
    
     // Get and print some values from the JSON
    if (jsonContent.contains("app_name") && jsonContent["app_name"].is_string()) 
    {
        std::string app_name = jsonContent["app_name"];
        std::cout << "app_name: " << app_name << "\n";
    }

    if (jsonContent.contains("version") && jsonContent["version"].is_number_integer()) 
    {
        int version = jsonContent["version"];
        std::cout << "version: " << version << "\n";
    }

    // Get nested object
    if (jsonContent.contains("window") && jsonContent["window"].is_object()) 
    {
        int w = jsonContent["window"].value("width", 640); 
        int h = jsonContent["window"].value("height", 480);
        std::cout << "window: " << w << "x" << h << "\n";
    }

    // Get array of objects
    if (jsonContent.contains("users") && jsonContent["users"].is_array()) 
    {
        for (const auto& user : jsonContent["users"]) 
        {
            int id = user.value("id", -1);
            std::string name = user.value("name", std::string("unknown"));
            std::cout << "user: id=" << id << " name=" << name << "\n";
        }
    }
}
