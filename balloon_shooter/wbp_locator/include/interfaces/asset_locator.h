#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace wbp_locator
{
    class ILocatorAsset : public wb::IAsset
    {
    public:
        virtual ~ILocatorAsset() = default;
        virtual std::vector<DirectX::XMFLOAT3> &GetLocates() = 0;
        virtual std::vector<DirectX::XMFLOAT3> &GetRotations() = 0;
    };

} // namespace wbp_locator