#pragma once
#include "windows_base/windows_base.h"

namespace $project_name$
{
    class $SceneName$EntitiesFactory : public wb::IEntitiesFactory
    {
    public:
        $SceneName$EntitiesFactory() = default;
        ~$SceneName$EntitiesFactory() override = default;

        void Create
        (
            wb::IAssetContainer &assetCont, 
            wb::IEntityContainer &entityCont, 
            wb::IComponentContainer &componentCont, 
            wb::IEntityIDView &entityIDView
        ) const override;
    };

} // namespace $project_name$