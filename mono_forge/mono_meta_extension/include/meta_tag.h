#pragma once

#include "mono_meta_extension/include/dll_config.h"
#include "mono_meta_extension/include/meta_tag_generator.h"

namespace mono_meta_extension
{

// The template class for tags of specific types
template <typename MetaTagTag>
class MetaTag
{
public:
    MetaTag() = default;
    virtual ~MetaTag() = default;

    // Get the unique ID of the tag type
    static uint64_t Get()
    {
        static const uint64_t tag = MetaTagGenerator::GetInstance().GenerateTag();
        return tag;
    }
};

} // namespace mono_meta_extension