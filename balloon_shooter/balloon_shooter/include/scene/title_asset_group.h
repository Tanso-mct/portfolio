#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class TitleAssetGroup : public wb::AssetGroup
    {
    public:
        TitleAssetGroup();
        ~TitleAssetGroup() override = default;
    };

} // namespace balloon_shooter