#include "windows_base/src/pch.h"
#include "windows_base/include/asset_registry.h"

void wb::AssetRegistry::Add(size_t id, size_t factoryID, size_t fileLoaderID, std::string_view filePath)
{
    if (factoryIDs_.find(id) != factoryIDs_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Asset with ID ", std::to_string(id), " already exists."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Add the asset factory, loader ID, and file path to the registrys
    factoryIDs_[id] = factoryID;
    fileLoaderIDs_[id] = fileLoaderID;
    filePaths_[id] = filePath.data();

    // Save the key
    keys_.push_back(id);

    // Update the maximum ID if necessary
    if (id > maxId)
    {
        maxId = id;
    }
}

const size_t &wb::AssetRegistry::GetFactoryID(size_t id) const
{
    if (factoryIDs_.find(id) == factoryIDs_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Asset factory for ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return factoryIDs_.at(id);
}

const size_t &wb::AssetRegistry::GetFileLoaderID(size_t id) const
{
    if (fileLoaderIDs_.find(id) == fileLoaderIDs_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Loader ID for asset with ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return fileLoaderIDs_.at(id);
}

std::string_view wb::AssetRegistry::GetFilePath(size_t id) const
{
    if (filePaths_.find(id) == filePaths_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"File path for asset with ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return filePaths_.at(id);
}

size_t wb::AssetRegistry::GetMaxID() const
{
    return maxId;
}

const std::vector<size_t> &wb::AssetRegistry::GetKeys() const
{
    return keys_;
}

WINDOWS_BASE_API wb::AssetRegistry wb::gAssetRegistry;

wb::AssetRegistrar::AssetRegistrar(size_t assetID, size_t factoryID, size_t fileLoaderID, std::string_view filePath)
{
    wb::gAssetRegistry.Add(assetID, factoryID, fileLoaderID, filePath);
}