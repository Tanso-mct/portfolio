#pragma once

#include "windows_base/include/interfaces/entity.h"

namespace wb
{
    class IPrefab
    {
    public:
        virtual ~IPrefab() = default;

        virtual std::unique_ptr<IOptionalValue> Create
        (
            IAssetContainer &assetCont,
            IEntityContainer &entityCont,
            IComponentContainer &componentCont,
            IEntityIDView &entityIDView
        ) const = 0;

        virtual std::vector<size_t> GetNeedAssetIDs() const = 0;
    };

} // namespace wb