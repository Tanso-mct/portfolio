#include "mono_file/src/pch.h"
#include "mono_file/include/fbx.h"

#include "utility_header/file_loader.h"

#pragma comment(lib, "riaecs.lib")

using namespace DirectX;

#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

namespace
{

void ProcessNodeOnlyTransform(FbxNode* node, mono_file::FileDataFBXTransformOnly& fbxData)
{
    if (!node) return;

    FbxMesh* mesh = node->GetMesh();
    if (mesh)
    {
        // メッシュ構造体を作成
        mono_file::FileDataFBXTransformOnly::Mesh meshData;

        // マテリアル情報
        FbxLayerElementMaterial* materialElement = mesh->GetElementMaterial();

        // メッシュ名
        meshData.name_ = node->GetName();

        // Transform情報
        FbxAMatrix transform = node->EvaluateGlobalTransform();

        // 変換情報を取得
        FbxVector4 translation = transform.GetT();
        FbxVector4 rotation = transform.GetR();
        FbxVector4 scale = transform.GetS();

        // Transform構造体を作成して追加
        mono_file::FileDataFBXTransformOnly::Transform transformData;
        transformData.name_ = node->GetName();
        transformData.translation_ = XMFLOAT3((float)translation[0], (float)translation[1], (float)translation[2]);
        transformData.rotation_ = XMFLOAT3((float)rotation[0], (float)rotation[1], (float)rotation[2]);
        transformData.scale_ = XMFLOAT3((float)scale[0], (float)scale[1], (float)scale[2]);
        if (node->GetParent())
            transformData.parentName_ = node->GetParent()->GetName();
        else
            transformData.parentName_.clear(); // 親なし

        // MeshDataを追加
        fbxData.AddTransform(std::move(transformData));
    }

    // 再帰
    int childCount = node->GetChildCount();
    for (int i = 0; i < childCount; ++i)
        ProcessNodeOnlyTransform(node->GetChild(i), fbxData);
}

void ProcessNodeOnlyMinMax(FbxNode* node, mono_file::FileDataFbxMinMaxOnly& fbxData)
{
    if (!node) return;

    FbxMesh* mesh = node->GetMesh();
    if (mesh)
    {
        FbxAMatrix transform = node->EvaluateGlobalTransform();
        int polygonCount = mesh->GetPolygonCount();

        // MinMaxを取得
        mono_file::FileDataFbxMinMaxOnly::MinMax minMax = fbxData.GetMinMax();

        // 頂点配列をポリゴン頂点単位で作成
        for (int pi = 0; pi < polygonCount; ++pi)
        {
            int polySize = mesh->GetPolygonSize(pi);

            for (int vi = 0; vi < polySize; ++vi)
            {
                int ctrlPointIndex = mesh->GetPolygonVertex(pi, vi);

                // 頂点情報作成

                // 位置
                FbxVector4 pos = transform.MultT(mesh->GetControlPointAt(ctrlPointIndex));
                XMFLOAT3 position((float)pos[0], (float)pos[1], (float)pos[2]);

                // Min更新
                if (position.x < minMax.min_.x) 
                    minMax.min_.x = position.x;
                if (position.y < minMax.min_.y) 
                    minMax.min_.y = position.y;
                if (position.z < minMax.min_.z) 
                    minMax.min_.z = position.z;

                // Max更新
                if (position.x > minMax.max_.x) 
                    minMax.max_.x = position.x;
                if (position.y > minMax.max_.y) 
                    minMax.max_.y = position.y;
                if (position.z > minMax.max_.z) 
                    minMax.max_.z = position.z;
            }
        }

        // MinMaxを設定
        fbxData.SetMinMax(minMax);
    }

    // 再帰
    int childCount = node->GetChildCount();
    for (int i = 0; i < childCount; ++i)
        ProcessNodeOnlyMinMax(node->GetChild(i), fbxData);
}

} // namespace

mono_file::FileDataFBX::FileDataFBX(std::unique_ptr<mono_forge_model::MFM> mfm)
    : mfm_(std::move(mfm))
{
    assert(mfm_ && "MFM data is null");
}

mono_file::FileDataFBX::~FileDataFBX()
{
}

mono_file::FileLoaderFBX::FileLoaderFBX()
{
}

mono_file::FileLoaderFBX::~FileLoaderFBX()
{
}

std::unique_ptr<riaecs::IFileData> mono_file::FileLoaderFBX::Load(std::string_view filePath) const
{
    // Load MFM file data
    std::unique_ptr<uint8_t[]> mfm_file_data = nullptr;
    fpos_t mfm_file_size = 0;
    mfm_file_data = utility_header::LoadFile(filePath, mfm_file_size);
    if (mfm_file_data == nullptr)
        return nullptr; // Failed to load file

    // Create MFM object
    std::unique_ptr<mono_forge_model::MFM> mfm 
        = std::make_unique<mono_forge_model::MFM>(
            std::move(mfm_file_data), static_cast<uint32_t>(mfm_file_size));

    // Create FileDataFBX object
    std::unique_ptr<FileDataFBX> fbxData = std::make_unique<FileDataFBX>(std::move(mfm));

    return fbxData; // Return the FBX file data
}

MONO_FILE_API riaecs::FileLoaderRegistrar<mono_file::FileLoaderFBX> mono_file::FileLoaderFBXID;

void mono_file::FileDataFBXTransformOnly::AddTransform(Transform&& transform)
{
    transforms_.emplace_back(std::move(transform));
}

std::unique_ptr<riaecs::IFileData> mono_file::FileLoaderFBXTransformOnly::Load(std::string_view filePath) const
{
    std::unique_ptr<FileDataFBXTransformOnly> fbxData = std::make_unique<FileDataFBXTransformOnly>();

    fbxsdk::FbxManager* fbxManager = nullptr;
    FbxScene* fbxScene = nullptr;
    FbxImporter* fbxImporter = nullptr;
    FbxNode* fbxNode = nullptr;

    fbxManager = fbxsdk::FbxManager::Create();

    fbxImporter = FbxImporter::Create(fbxManager, filePath.data());
    if (fbxImporter == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to create FbxImporter.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    fbxScene = FbxScene::Create(fbxManager, filePath.data());
    if (fbxScene == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to create FbxScene.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    bool res = fbxImporter->Initialize(filePath.data());
    if (res == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to initialize FbxImporter.",
            "File path: " + std::string(filePath),
            fbxImporter->GetStatus().GetErrorString()
        }, RIAECS_LOG_LOC);
    }

    // sceneにインポート
    if (fbxImporter->Import(fbxScene) == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to import FbxScene.",
            "File path: " + std::string(filePath),
            fbxImporter->GetStatus().GetErrorString()
        }, RIAECS_LOG_LOC);
    }

    FbxGeometryConverter converter(fbxManager);
    converter.Triangulate(fbxScene, true);

    FbxAxisSystem dx = FbxAxisSystem::DirectX;
    if (fbxScene->GetGlobalSettings().GetAxisSystem() != dx)
        dx.DeepConvertScene(fbxScene);

    fbxNode = fbxScene->GetRootNode();
    FbxNode* rootNode = fbxScene->GetRootNode();
    if (rootNode)
        ProcessNodeOnlyTransform(rootNode, *fbxData);
    else
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "FbxScene has no root node.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    fbxImporter->Destroy();
    fbxScene->Destroy();
    fbxManager->Destroy();

    return fbxData;
}

MONO_FILE_API riaecs::FileLoaderRegistrar<mono_file::FileLoaderFBXTransformOnly> mono_file::FileLoaderFBXTransformOnlyID;

std::unique_ptr<riaecs::IFileData> mono_file::FileLoaderFbxMinMaxOnly::Load(std::string_view filePath) const
{
    std::unique_ptr<FileDataFbxMinMaxOnly> fbxData = std::make_unique<FileDataFbxMinMaxOnly>();

    fbxsdk::FbxManager* fbxManager = nullptr;
    FbxScene* fbxScene = nullptr;
    FbxImporter* fbxImporter = nullptr;
    FbxNode* fbxNode = nullptr;

    fbxManager = fbxsdk::FbxManager::Create();

    fbxImporter = FbxImporter::Create(fbxManager, filePath.data());
    if (fbxImporter == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to create FbxImporter.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    fbxScene = FbxScene::Create(fbxManager, filePath.data());
    if (fbxScene == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to create FbxScene.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    bool res = fbxImporter->Initialize(filePath.data());
    if (res == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to initialize FbxImporter.",
            "File path: " + std::string(filePath),
            fbxImporter->GetStatus().GetErrorString()
        }, RIAECS_LOG_LOC);
    }

    // sceneにインポート
    if (fbxImporter->Import(fbxScene) == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "Failed to import FbxScene.",
            "File path: " + std::string(filePath),
            fbxImporter->GetStatus().GetErrorString()
        }, RIAECS_LOG_LOC);
    }

    FbxGeometryConverter converter(fbxManager);
    converter.Triangulate(fbxScene, true);

    FbxAxisSystem dx = FbxAxisSystem::DirectX;
    if (fbxScene->GetGlobalSettings().GetAxisSystem() != dx)
        dx.DeepConvertScene(fbxScene);

    fbxNode = fbxScene->GetRootNode();
    FbxNode* rootNode = fbxScene->GetRootNode();
    if (rootNode)
        ProcessNodeOnlyMinMax(rootNode, *fbxData);
    else
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        riaecs::NotifyError({
            "FbxScene has no root node.",
            "File path: " + std::string(filePath)
        }, RIAECS_LOG_LOC);
    }

    fbxImporter->Destroy();
    fbxScene->Destroy();
    fbxManager->Destroy();

    return fbxData;
}

MONO_FILE_API riaecs::FileLoaderRegistrar<mono_file::FileLoaderFbxMinMaxOnly> mono_file::FileLoaderFbxMinMaxOnlyID;