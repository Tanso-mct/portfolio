#pragma once
#include "mono_file/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <string>
#include <vector>
#include <DirectXMath.h>

#include "mono_forge_model/include/mfm.h"

namespace mono_file
{
    class MONO_FILE_API FileDataFBX : public riaecs::IFileData
    {
    public:
        const mono_forge_model::MFM& GetMFM() const { return *mfm_; }

    private:
        std::unique_ptr<mono_forge_model::MFM> mfm_ = nullptr;

    public:
        FileDataFBX(std::unique_ptr<mono_forge_model::MFM> mfm);
        ~FileDataFBX() override;
    };

    class MONO_FILE_API FileLoaderFBX : public riaecs::IFileLoader
    {
    public:
        FileLoaderFBX();
        ~FileLoaderFBX() override;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };
    extern MONO_FILE_API riaecs::FileLoaderRegistrar<FileLoaderFBX> FileLoaderFBXID;

    // Only transform fbx data
    class MONO_FILE_API FileDataFBXTransformOnly : public riaecs::IFileData
    {
    public:
        FileDataFBXTransformOnly() = default;
        ~FileDataFBXTransformOnly() override = default;

        struct Vertex
        {
            DirectX::XMFLOAT3 position_;
            DirectX::XMFLOAT3 normal_;
            DirectX::XMFLOAT4 color_;
            DirectX::XMFLOAT2 uv_;
        };

        struct Mesh
        {
            std::string name_;
            std::vector<Vertex> vertices_;
            std::vector<unsigned int> indices_;
            std::string materialName_;
        };

        struct Transform
        {
            std::string name_;
            DirectX::XMFLOAT3 translation_;
            DirectX::XMFLOAT3 rotation_;
            DirectX::XMFLOAT3 scale_;
            std::string parentName_; // Empty if no parent
        };

        void AddTransform(Transform &&transform);
        const std::vector<Transform>& GetTransforms() const { return transforms_; }

    private:
        std::vector<Transform> transforms_;
    };

    class MONO_FILE_API FileLoaderFBXTransformOnly : public riaecs::IFileLoader
    {
    public:
        FileLoaderFBXTransformOnly() = default;
        ~FileLoaderFBXTransformOnly() override = default;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };
    extern MONO_FILE_API riaecs::FileLoaderRegistrar<FileLoaderFBXTransformOnly> FileLoaderFBXTransformOnlyID;

    class MONO_FILE_API FileDataFbxMinMaxOnly : public riaecs::IFileData
    {
    public:
        FileDataFbxMinMaxOnly() = default;
        ~FileDataFbxMinMaxOnly() override = default;

        struct MinMax
        {
            DirectX::XMFLOAT3 min_ = {FLT_MAX, FLT_MAX, FLT_MAX};
            DirectX::XMFLOAT3 max_ = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
        };

        void SetMinMax(const MinMax &minMax) { minMax_ = minMax; }
        const MinMax& GetMinMax() const { return minMax_; }

    private:
        MinMax minMax_;
    };

    class MONO_FILE_API FileLoaderFbxMinMaxOnly : public riaecs::IFileLoader
    {
    public:
        FileLoaderFbxMinMaxOnly() = default;
        ~FileLoaderFbxMinMaxOnly() override = default;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };

    extern MONO_FILE_API riaecs::FileLoaderRegistrar<FileLoaderFbxMinMaxOnly> FileLoaderFbxMinMaxOnlyID;


} // namespace mono_file