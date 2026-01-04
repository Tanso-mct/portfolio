#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class LoadAssetGroup : public wb::AssetGroup
    {
    public:
        LoadAssetGroup();
        ~LoadAssetGroup() override = default;
    };

} // namespace balloon_shooter