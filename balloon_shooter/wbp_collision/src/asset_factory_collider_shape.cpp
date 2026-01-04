#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/asset_factory_collider_shape.h"

#include "wbp_collision/include/asset_collider_shape.h"

#include "wbp_primitive/include/primitive_helpers.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

const WBP_COLLISION_API size_t &wbp_collision::ColliderShapeAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

std::unique_ptr<wb::IAsset> wbp_collision::ColliderShapeAssetFactory::Create(wb::IFileData &fileData) const
{
    // Check if fileData is of type fbx
    wbp_fbx_loader::IFBXFileData* fbxFileData = wb::As<wbp_fbx_loader::IFBXFileData>(&fileData);
    if (!fbxFileData)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Invalid file data type. Expected IFBXFileData."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }

    std::unique_ptr<wbp_collision::IColliderShapeAsset> colliderShapeAsset 
        = std::make_unique<wbp_collision::ColliderShapeAsset>();

    if (fbxFileData->GetMeshes().empty())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "FBX file does not contain any meshes."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }

    for (const wbp_fbx_loader::FBXMesh &mesh : fbxFileData->GetMeshes())
    {
        // Set vertices from mesh
        XMFLOAT3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
        XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
        for (const wbp_fbx_loader::FBXVertex &vertex : mesh.vertices_)
        {
            colliderShapeAsset->GetVertices().emplace_back(vertex.position_);
            
            if (vertex.position_.x < min.x) min.x = vertex.position_.x;
            if (vertex.position_.y < min.y) min.y = vertex.position_.y;
            if (vertex.position_.z < min.z) min.z = vertex.position_.z;

            if (vertex.position_.x > max.x) max.x = vertex.position_.x;
            if (vertex.position_.y > max.y) max.y = vertex.position_.y;
            if (vertex.position_.z > max.z) max.z = vertex.position_.z;
        }

        // Set AABB from vertices
        colliderShapeAsset->GetAABBs().emplace_back(wbp_primitive::PrimitiveAABB(min, max));
    }

    // Create AABB from all AABBs
    colliderShapeAsset->GetTotalAABB() = wbp_primitive::CreateAABBFromAABBs(colliderShapeAsset->GetAABBs());

    // Cast to IAsset
    std::unique_ptr<wb::IAsset> asset = wb::UniqueAs<wb::IAsset>(colliderShapeAsset);
    return asset;
}

namespace wbp_collision
{
    WB_REGISTER_ASSET_FACTORY(ColliderShapeAssetFactoryID(), ColliderShapeAssetFactory);

} // namespace wbp_collision