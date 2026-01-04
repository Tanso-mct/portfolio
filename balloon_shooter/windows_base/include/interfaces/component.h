#pragma once

namespace wb
{
    class IComponent
    {
    public:
        virtual ~IComponent() = default;
        virtual const size_t &GetID() const = 0;
    };

} // namespace wb