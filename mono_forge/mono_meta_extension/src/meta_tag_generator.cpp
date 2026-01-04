#include "mono_meta_extension/src/pch.h"
#include "mono_meta_extension/include/meta_tag_generator.h"

namespace mono_meta_extension
{

uint64_t MetaTagGenerator::GenerateTag()
{
    assert(current_tag_ < MAX_TAG && "Exceeded maximum tag limit");
    return current_tag_++;
}

} // namespace mono_meta_extension