#include "windows_base/src/pch.h"
#include "windows_base/include/file_loader_registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

void wb::FileLoaderRegistry::AddLoader(size_t id, std::unique_ptr<IFileLoader> loader)
{
    if (fileLoaders_.find(id) != fileLoaders_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"File loader with ID ", std::to_string(id), " already exists."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Add the loader to the registry
    fileLoaders_[id] = std::move(loader);

    // Save the key
    keys_.push_back(id);

    // Update the maximum ID if necessary
    if (id > maxId)
    {
        maxId = id;
    }
}

wb::IFileLoader &wb::FileLoaderRegistry::GetLoader(size_t id)
{
    if (fileLoaders_.find(id) == fileLoaders_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"File loader with ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *fileLoaders_[id];
}

size_t wb::FileLoaderRegistry::GetMaxID() const
{
    return maxId;
}

const std::vector<size_t> &wb::FileLoaderRegistry::GetKeys() const
{
    return keys_;
}

WINDOWS_BASE_API wb::FileLoaderRegistry wb::gFileLoaderRegistry;

wb::FileLoaderRegistrar::FileLoaderRegistrar(size_t id, std::unique_ptr<IFileLoader> loader)
{
    wb::gFileLoaderRegistry.AddLoader(id, std::move(loader));
}