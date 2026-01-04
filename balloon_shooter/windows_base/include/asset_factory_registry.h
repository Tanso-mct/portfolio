#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/asset.h"
#include "windows_base/include/interfaces/registry.h"

namespace wb
{
    class WINDOWS_BASE_API AssetFactoryRegistry : public IAssetFactoryRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<IAssetFactory>> factories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        AssetFactoryRegistry() = default;
        virtual ~AssetFactoryRegistry() = default;

        AssetFactoryRegistry(const AssetFactoryRegistry &) = delete;
        AssetFactoryRegistry &operator=(const AssetFactoryRegistry &) = delete;

        /***************************************************************************************************************
         * IAssetFactoryRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t id, std::unique_ptr<IAssetFactory> factory) override;
        IAssetFactory &GetFactory(size_t id) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API AssetFactoryRegistry gAssetFactoryRegistry;

    class WINDOWS_BASE_API AssetFactoryRegistrar
    {
    public:
        AssetFactoryRegistrar(size_t id, std::unique_ptr<IAssetFactory> factory);
    };

} // namespace wb

#define WB_REGISTER_ASSET_FACTORY(ID, FACTORY) \
    static wb::AssetFactoryRegistrar gAssetFactoryRegistrar##FACTORY(ID, std::make_unique<FACTORY>());