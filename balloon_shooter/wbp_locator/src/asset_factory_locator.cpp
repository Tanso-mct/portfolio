#include "wbp_locator/src/pch.h"
#include "wbp_locator/include/asset_factory_locator.h"

#include "wbp_locator/include/asset_locator.h"

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

using namespace DirectX;

const WBP_LOCATOR_API size_t &wbp_locator::LocatorAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

std::unique_ptr<wb::IAsset> wbp_locator::LocatorAssetFactory::Create(wb::IFileData &fileData) const
{
    // Check if fileData is of type fbx
    wbp_fbx_loader::IFBXFileData* fbxFileData = wb::As<wbp_fbx_loader::IFBXFileData>(&fileData);
    wbp_fbx_loader::IFBXTransformFileData* fbxTransformFileData = wb::As<wbp_fbx_loader::IFBXTransformFileData>(&fileData);
    if (!fbxFileData && !fbxTransformFileData)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Invalid file data type. Expected IFBXFileData or IFBXTransformFileData.",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_MODEL", err);
        wb::ThrowRuntimeError(err);
    }

    std::unique_ptr<wbp_locator::ILocatorAsset> locatorAsset = std::make_unique<wbp_locator::LocatorAsset>();

    if (fbxFileData)
    {
        for (wbp_fbx_loader::FBXMesh &mesh : fbxFileData->GetMeshes())
        {
            XMFLOAT3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
            XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

            for (const wbp_fbx_loader::FBXVertex &vertex : mesh.vertices_)
            {
                if (vertex.position_.x < min.x) min.x = vertex.position_.x;
                if (vertex.position_.y < min.y) min.y = vertex.position_.y;
                if (vertex.position_.z < min.z) min.z = vertex.position_.z;

                if (vertex.position_.x > max.x) max.x = vertex.position_.x;
                if (vertex.position_.y > max.y) max.y = vertex.position_.y;
                if (vertex.position_.z > max.z) max.z = vertex.position_.z;
            }

            // Calculate the center of the mesh
            XMFLOAT3 center;
            center.x = (min.x + max.x) / 2.0f;
            center.y = (min.y + max.y) / 2.0f;
            center.z = (min.z + max.z) / 2.0f;

            // Add the center to the locates
            locatorAsset->GetLocates().emplace_back(center);
            locatorAsset->GetRotations().emplace_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
        }
    }
    else if (fbxTransformFileData)
    {
        for (const wbp_fbx_loader::FBXTransform &transform : fbxTransformFileData->GetTransforms())
        {
            // Add the transform position to the locates
            locatorAsset->GetLocates().emplace_back(transform.position_);
            locatorAsset->GetRotations().emplace_back(transform.rotation_);
        }
    }

    // Cast to IAsset
    std::unique_ptr<wb::IAsset> asset = wb::UniqueAs<wb::IAsset>(locatorAsset);
    return asset;
}

namespace wbp_locator
{
    WB_REGISTER_ASSET_FACTORY(LocatorAssetFactoryID(), LocatorAssetFactory);

} // namespace wbp_locator