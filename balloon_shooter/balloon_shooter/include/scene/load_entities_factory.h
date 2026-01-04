#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class LoadEntitiesFactory : public wb::IEntitiesFactory
    {
    public:
        LoadEntitiesFactory() = default;
        ~LoadEntitiesFactory() override = default;

        void Create
        (
            wb::IAssetContainer &assetCont, 
            wb::IEntityContainer &entityCont, 
            wb::IComponentContainer &componentCont, 
            wb::IEntityIDView &entityIDView
        ) const override;
    };

} // namespace balloon_shooter