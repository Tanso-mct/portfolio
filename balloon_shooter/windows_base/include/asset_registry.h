#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/asset.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API AssetRegistry : public IAssetRegistry
    {
    private:
        std::unordered_map<size_t, size_t> factoryIDs_;
        std::unordered_map<size_t, size_t> fileLoaderIDs_;
        std::unordered_map<size_t, std::string> filePaths_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        AssetRegistry() = default;
        virtual ~AssetRegistry() = default;

        AssetRegistry(const AssetRegistry &) = delete;
        AssetRegistry &operator=(const AssetRegistry &) = delete;

        /***************************************************************************************************************
         * IAssetRegistry implementation
        /**************************************************************************************************************/

        void Add(size_t id, size_t factoryID, size_t fileLoaderID, std::string_view filePath) override;
        const size_t &GetFactoryID(size_t id) const override;
        const size_t &GetFileLoaderID(size_t id) const override;
        std::string_view GetFilePath(size_t id) const override;
        
        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API AssetRegistry gAssetRegistry;

    class WINDOWS_BASE_API AssetRegistrar
    {
    public:
        AssetRegistrar(size_t assetID, size_t factoryID, size_t fileLoaderID, std::string_view filePath);
    };

} // namespace wb

#define WB_REGISTER_ASSET(ID_FUNC, FACTORY_ID, FILE_LOADER_ID, FILE_PATH) \
    static wb::AssetRegistrar assetRegistrar##ID_FUNC(ID_FUNC(), FACTORY_ID, FILE_LOADER_ID, FILE_PATH);