#pragma once
#include "wbp_fbx_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_fbx_loader
{
    const WBP_FBX_LOADER_API size_t &FBXTransformFileLoaderID();

    class WBP_FBX_LOADER_API FBXTransformFileLoader : public wb::IFileLoader
    {
    public:
        FBXTransformFileLoader() = default;
        virtual ~FBXTransformFileLoader() = default;

        const size_t &GetID() const override;
        std::unique_ptr<wb::IFileData> Load(std::string_view path) override;
    };

} // namespace wbp_fbx_loader