#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class PlayEntitiesFactory : public wb::IEntitiesFactory
    {
    public:
        PlayEntitiesFactory() = default;
        ~PlayEntitiesFactory() override = default;

        void Create
        (
            wb::IAssetContainer &assetCont, 
            wb::IEntityContainer &entityCont, 
            wb::IComponentContainer &componentCont, 
            wb::IEntityIDView &entityIDView
        ) const override;
    };

} // namespace balloon_shooter