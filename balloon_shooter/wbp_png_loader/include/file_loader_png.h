#pragma once
#include "wbp_png_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_png_loader
{
    const WBP_PNG_LOADER_API size_t &PNGFileLoaderID();

    class WBP_PNG_LOADER_API PNGFileLoader : public wb::IFileLoader
    {
    public:
        PNGFileLoader() = default;
        virtual ~PNGFileLoader() = default;

        const size_t &GetID() const override;
        std::unique_ptr<wb::IFileData> Load(std::string_view path) override;
    };

} // namespace wbp_png_loader