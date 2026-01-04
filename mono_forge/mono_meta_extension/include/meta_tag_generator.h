#pragma once

#include "class_template/singleton.h"

#include "mono_meta_extension/include/dll_config.h"

namespace mono_meta_extension
{

// Initial tag value
constexpr uint64_t INITIAL_TAG = 0;

// Max tag value
// Tag uses bit flags, so the maximum is 64 bits
constexpr uint64_t MAX_TAG = 64;

// The singleton tag generator class
class MONO_META_EXT_DLL MetaTagGenerator :
    public class_template::Singleton<MetaTagGenerator>
{
public:
    MetaTagGenerator() = default;
    virtual ~MetaTagGenerator() = default;

    // Generate a new unique tag
    uint64_t GenerateTag();

private:
    uint64_t current_tag_ = INITIAL_TAG; // The current maximum tag value
};

} // namespace mono_meta_extension