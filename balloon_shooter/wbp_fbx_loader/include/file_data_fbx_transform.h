#pragma once
#include "wbp_fbx_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_fbx_loader/include/interfaces/file_data_fbx_transform.h"

namespace wbp_fbx_loader
{
    class WBP_FBX_LOADER_API FileDataFBXTransform : public IFBXTransformFileData
    {
    private:
        std::vector<FBXTransform> transforms_;

    public:
        FileDataFBXTransform() = default;
        ~FileDataFBXTransform() override = default;

        void AddTransform(const FBXTransform &transform) { transforms_.emplace_back(transform); }
        const std::vector<FBXTransform> &GetTransforms() const override { return transforms_; }
    };

} // namespace wbp_fbx_loader