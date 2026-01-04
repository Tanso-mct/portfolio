#pragma once

namespace class_template
{
    // A generic factory interface for creating and destroying products of type PRODUCT with optional arguments ARGS.
    template <typename PRODUCT, typename... ARGS>
    class IFactory
    {
    public:
        virtual ~IFactory() = default;
        
        virtual PRODUCT Create(ARGS...) const = 0;
        virtual void Destroy(PRODUCT& product) const = 0;
        virtual size_t GetProductSize() const = 0;
    };

} // namespace class_template