#pragma once
#include "wbp_fbx_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_fbx_loader
{
    const WBP_FBX_LOADER_API size_t &FBXFileLoaderID();

    class WBP_FBX_LOADER_API FBXFileLoader : public wb::IFileLoader
    {
    public:
        FBXFileLoader() = default;
        virtual ~FBXFileLoader() = default;

        const size_t &GetID() const override;
        std::unique_ptr<wb::IFileData> Load(std::string_view path) override;
    };

} // namespace wbp_fbx_loader