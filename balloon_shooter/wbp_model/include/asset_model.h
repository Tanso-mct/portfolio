#pragma once
#include "wbp_model/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_model/include/interfaces/asset_model.h"

namespace wbp_model
{
    class ModelAsset : public IModelAsset
    {
    private:
        std::vector<MeshGPUData> meshDatas_;

    public:
        ModelAsset() = default;
        ~ModelAsset() override = default;

        std::vector<MeshGPUData> &GetMeshDatas() override { return meshDatas_; }
    };

} // namespace wbp_model