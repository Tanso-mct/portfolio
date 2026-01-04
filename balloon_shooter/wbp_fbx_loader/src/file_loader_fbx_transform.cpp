#include "wbp_fbx_loader/src/pch.h"
#include "wbp_fbx_loader/include/file_loader_fbx_transform.h"

#include "wbp_fbx_loader/include/file_data_fbx_transform.h"

#include <DirectXMath.h>
using namespace DirectX;

#include <array>

#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

const WBP_FBX_LOADER_API size_t &wbp_fbx_loader::FBXTransformFileLoaderID()
{
    static size_t id = wb::IDFactory::CreateFileLoaderID();
    return id;
}

const size_t &wbp_fbx_loader::FBXTransformFileLoader::GetID() const
{
    return wbp_fbx_loader::FBXTransformFileLoaderID();
}

namespace
{
    void ProcessNode(FbxNode* node, wbp_fbx_loader::IFBXTransformFileData& fbxData)
    {
        if (!node) return;

        if (node->GetMesh())
        {
            // ここでメッシュの有無に関係なくノードのTransformを取得
            FbxAMatrix transform = node->EvaluateGlobalTransform();

            // 変換情報を取得
            FbxVector4 translation = transform.GetT();   // 平行移動
            FbxVector4 rotation = transform.GetR();      // 回転（オイラー角）
            FbxVector4 scaling = transform.GetS();       // スケール

            wbp_fbx_loader::FBXTransform transformData;
            transformData.position_ = XMFLOAT3
            (
                static_cast<float>(translation[0]),
                static_cast<float>(translation[1]),
                static_cast<float>(translation[2])
            );
            transformData.rotation_ = XMFLOAT3
            (
                static_cast<float>(rotation[0]),
                static_cast<float>(rotation[1]),
                static_cast<float>(rotation[2])
            );
            transformData.scale_ = XMFLOAT3
            (
                static_cast<float>(scaling[0]),
                static_cast<float>(scaling[1]),
                static_cast<float>(scaling[2])
            );

            fbxData.AddTransform(std::move(transformData));
        }

        // 子ノードも再帰処理
        int childCount = node->GetChildCount();
        for (int i = 0; i < childCount; ++i)
        {
            ProcessNode(node->GetChild(i), fbxData);
        }
    }
}

std::unique_ptr<wb::IFileData> wbp_fbx_loader::FBXTransformFileLoader::Load(std::string_view path)
{
    std::unique_ptr<wbp_fbx_loader::IFBXTransformFileData> fbxFileData = std::make_unique<wbp_fbx_loader::FileDataFBXTransform>();
    HRESULT hr = E_FAIL;

    fbxsdk::FbxManager* fbxManager = nullptr;
    FbxScene* fbxScene = nullptr;
    FbxImporter* fbxImporter = nullptr;
    FbxNode* fbxNode = nullptr;

    fbxManager = fbxsdk::FbxManager::Create();

    fbxImporter = FbxImporter::Create(fbxManager, path.data());
    if (fbxImporter == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Failed to create FbxImporter.",
                "File path: " + std::string(path)
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_FBX_LOADER", err);
        wb::ThrowRuntimeError(err);
    }

    fbxScene = FbxScene::Create(fbxManager, path.data());
    if (fbxScene == nullptr)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Failed to create FbxScene.",
                "File path: " + std::string(path)
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_FBX_LOADER", err);
        wb::ThrowRuntimeError(err);
    }

    bool res = fbxImporter->Initialize(path.data());
    if (res == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Failed to initialize FbxImporter.",
                "File path: " + std::string(path),
                fbxImporter->GetStatus().GetErrorString()
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_FBX_LOADER", err);
        wb::ThrowRuntimeError(err);
    }

    // sceneにインポート
    if (fbxImporter->Import(fbxScene) == false)
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Failed to import FbxScene.",
                "File path: " + std::string(path),
                fbxImporter->GetStatus().GetErrorString()
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_FBX_LOADER", err);
        wb::ThrowRuntimeError(err);
    }

    FbxGeometryConverter converter(fbxManager);
    converter.Triangulate(fbxScene, true);

    int animCount = fbxImporter->GetAnimStackCount();

    FbxAxisSystem dx = FbxAxisSystem::DirectX;
    if (fbxScene->GetGlobalSettings().GetAxisSystem() != dx)
    {
        dx.DeepConvertScene(fbxScene);
    }

    fbxNode = fbxScene->GetRootNode();
    FbxNode* rootNode = fbxScene->GetRootNode();
    if (rootNode)
    {
        ProcessNode(rootNode, *fbxFileData);
    }
    else
    {
        fbxImporter->Destroy();
        fbxScene->Destroy();
        fbxManager->Destroy();

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Failed to get root node from FbxScene.",
                "File path: " + std::string(path)
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_FBX_LOADER", err);
        wb::ThrowRuntimeError(err);
    }

    fbxImporter->Destroy();
    fbxScene->Destroy();
    fbxManager->Destroy();

    // Cast to IFileData
    std::unique_ptr<wb::IFileData> fileData = wb::UniqueAs<wb::IFileData>(fbxFileData);
    return fileData;
}

namespace wbp_fbx_loader
{
    WB_REGISTER_FILE_LOADER(FBXTransformFileLoaderID(), FBXTransformFileLoader);

} // namespace wbp_fbx_loader