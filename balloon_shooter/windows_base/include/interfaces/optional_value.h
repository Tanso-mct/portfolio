#pragma once

#include <memory>

namespace wb
{
    class IOptionalValue
    {
    public:
        virtual ~IOptionalValue() = default;

        virtual const size_t &operator()() const = 0;
        virtual bool IsValid() const = 0;

        virtual std::unique_ptr<IOptionalValue> Clone() const = 0;
    };

} // namespace wb