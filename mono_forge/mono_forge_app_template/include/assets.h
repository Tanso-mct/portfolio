#pragma once

#include <DirectXMath.h>

#include "asset_loader/include/asset_handle.h"

namespace mono_forge_app_template 
{

// A empty texture asset handle for testing purposes
class EmptyTextureAssetHandle : public asset_loader::AssetHandle<EmptyTextureAssetHandle> {};

// A box mesh asset handle
class BoxMeshAssetHandle : public asset_loader::AssetHandle<BoxMeshAssetHandle> {};
constexpr const char* BOX_MESH_ASSET_PATH = "../resources/mono_forge_app_template/box.mfm";
constexpr DirectX::XMFLOAT4 BOX_DEFAULT_BASE_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr float BOX_DEFAULT_ROUGHNESS_VALUE = 1.0f;
constexpr float BOX_DEFAULT_METALNESS_VALUE = 0.0f;
constexpr DirectX::XMFLOAT4 BOX_DEFAULT_EMISSION_COLOR = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

} // namespace mono_forge_app_template