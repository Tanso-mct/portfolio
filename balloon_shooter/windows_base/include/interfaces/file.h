#pragma once

#include <memory>
#include <string_view>

namespace wb
{
    class IFileData
    {
    public:
        virtual ~IFileData() = default;
    };

    class IFileLoader
    {
    public:
        virtual ~IFileLoader() = default;
        virtual const size_t &GetID() const = 0;
        virtual std::unique_ptr<IFileData> Load(std::string_view path) = 0;
    };

} // namespace wb