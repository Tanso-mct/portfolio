#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class PlayAssetGroup : public wb::AssetGroup
    {
    public:
        PlayAssetGroup();
        ~PlayAssetGroup() override = default;
    };

} // namespace balloon_shooter