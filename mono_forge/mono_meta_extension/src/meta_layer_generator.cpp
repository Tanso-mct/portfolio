#include "mono_meta_extension/src/pch.h"
#include "mono_meta_extension/include/meta_layer_generator.h"

namespace mono_meta_extension
{

uint64_t MetaLayerGenerator::GenerateLayer()
{
    assert(current_layer_ < MAX_LAYER && "Exceeded maximum layer limit");
    return current_layer_++;
}

} // namespace mono_meta_extension