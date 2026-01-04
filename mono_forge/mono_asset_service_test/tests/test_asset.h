#pragma once

#include "asset_loader/include/asset.h"

namespace asset_loader_test
{

// A simple test asset class for unit testing
class TestAsset : public asset_loader::Asset
{
public:
    TestAsset() = default;
    virtual ~TestAsset() override = default;

    int value = 0;
};

} // namespace asset_loader_test