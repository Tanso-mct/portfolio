#pragma once
#include "wbp_fbx_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_fbx_loader/include/interfaces/file_data_fbx.h"

namespace wbp_fbx_loader
{
    class FileDataFBX : public IFBXFileData
    {
    private:
        std::vector<FBXMesh> meshes_;

    public:
        FileDataFBX() = default;
        virtual ~FileDataFBX() = default;

        std::vector<FBXMesh> &GetMeshes() override { return meshes_; }

    };

} // namespace wbp_fbx_loader