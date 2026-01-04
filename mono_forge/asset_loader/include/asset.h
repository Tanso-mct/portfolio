#pragma once

#include <string>
#include <string_view>

#include "class_template/non_copy.h"

namespace asset_loader
{

constexpr const char* DEFFAULT_ASSET_NAME = "Unnamed";
constexpr const char* DEFAULT_ASSET_TYPE_NAME = "Unknown";

// The interface for all assets
// Assets can be anything from textures, models, sounds, etc.
class Asset :
    public class_template::NonCopyable
{
public:
    virtual ~Asset() = default;

    // Get the name of the asset
    std::string_view GetName() const { return name_; }

    // Set the name of the asset
    void SetName(const std::string& name) const { name_ = name; }

    // Get the type name of the asset
    virtual std::string_view GetTypeName() const { return DEFAULT_ASSET_TYPE_NAME; }

private:
    mutable std::string name_ = DEFFAULT_ASSET_NAME;
};

} // namespace asset_loader