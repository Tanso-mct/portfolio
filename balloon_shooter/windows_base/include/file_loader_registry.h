#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/file.h"
#include "windows_base/include/interfaces/registry.h"

namespace wb
{
    class WINDOWS_BASE_API FileLoaderRegistry : public IFileLoaderRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<IFileLoader>> fileLoaders_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        FileLoaderRegistry() = default;
        virtual ~FileLoaderRegistry() = default;

        FileLoaderRegistry(const FileLoaderRegistry &) = delete;
        FileLoaderRegistry &operator=(const FileLoaderRegistry &) = delete;

        /***************************************************************************************************************
         * IFileLoaderRegistry implementation
        /**************************************************************************************************************/

        void AddLoader(size_t type, std::unique_ptr<IFileLoader> loader) override;
        IFileLoader &GetLoader(size_t type) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API FileLoaderRegistry gFileLoaderRegistry;

    class WINDOWS_BASE_API FileLoaderRegistrar
    {
    public:
        FileLoaderRegistrar(size_t id, std::unique_ptr<IFileLoader> loader);
    };

} // namespace wb

#define WB_REGISTER_FILE_LOADER(ID, LOADER) \
    static wb::FileLoaderRegistrar gFileLoaderRegistrar##LOADER(ID, std::make_unique<LOADER>());