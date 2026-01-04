#pragma once

#include "class_template/singleton.h"

#include "mono_meta_extension/include/dll_config.h"

namespace mono_meta_extension
{

// Initial layer value
constexpr uint64_t INITIAL_LAYER = 0;

// Max layer value
// Layer uses bit flags, so the maximum is 64 bits
constexpr uint64_t MAX_LAYER = 64;

// The singleton layer generator class
class MONO_META_EXT_DLL MetaLayerGenerator :
    public class_template::Singleton<MetaLayerGenerator>
{
public:
    MetaLayerGenerator() = default;
    virtual ~MetaLayerGenerator() = default;

    // Generate a new unique layer
    uint64_t GenerateLayer();

private:
    uint64_t current_layer_ = INITIAL_LAYER; // The current maximum layer value
};

} // namespace mono_meta_extension