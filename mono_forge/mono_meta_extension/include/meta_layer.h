#pragma once

#include "mono_meta_extension/include/dll_config.h"
#include "mono_meta_extension/include/meta_layer_generator.h"

namespace mono_meta_extension
{

// The template class for layers of specific types
template <typename MetaLayerTag>
class MetaLayer
{
public:
    MetaLayer() = default;
    virtual ~MetaLayer() = default;

    // Get the unique ID of the tag type
    static uint64_t Get()
    {
        static const uint64_t tag = MetaLayerGenerator::GetInstance().GenerateLayer();
        return tag;
    }
};

} // namespace mono_meta_extension