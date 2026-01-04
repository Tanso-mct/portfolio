#pragma once

#include <cstddef>
#include <memory>

#include "class_template/non_copy.h"
#include "memory_allocator/include/dll_config.h"

namespace memory_allocator
{

class MEMORY_ALLOCATOR_DLL Pool :
    public class_template::NonCopyable
{
public:
    Pool(size_t pool_size);
    virtual ~Pool() = default;

    std::byte* Get() { return pool_.get(); }
    size_t GetSize() const { return pool_size_; }

private:
    const std::unique_ptr<std::byte[]> pool_;
    const size_t pool_size_;
};

} // namespace memory_allocator