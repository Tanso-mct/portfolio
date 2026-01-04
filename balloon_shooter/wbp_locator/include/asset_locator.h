#pragma once
#include "wbp_locator/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_locator/include/interfaces/asset_locator.h"

namespace wbp_locator
{
    class LocatorAsset : public ILocatorAsset
    {
    private:
        std::vector<DirectX::XMFLOAT3> locates_;
        std::vector<DirectX::XMFLOAT3> rotations_;

    public:
        LocatorAsset() = default;
        ~LocatorAsset() override = default;

        std::vector<DirectX::XMFLOAT3> &GetLocates() override { return locates_; }
        std::vector<DirectX::XMFLOAT3> &GetRotations() override { return rotations_; }
    };

} // namespace wbp_locator