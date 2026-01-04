#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/asset.h"

#include <vector>

namespace wb
{
    class WINDOWS_BASE_API AssetGroup : public IAssetGroup
    {
    private:
        std::vector<size_t> assetIDs_;

    public:
        AssetGroup() = default;
        virtual ~AssetGroup() = default;

        /***************************************************************************************************************
         * IAssetGroup implementation
        /**************************************************************************************************************/

        const std::vector<size_t> &GetAssetIDs() const final;

    protected:
        void AddAssetID(size_t assetID) final;
    };

} // namespace wb