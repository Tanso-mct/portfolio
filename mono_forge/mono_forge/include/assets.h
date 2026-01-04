#pragma once

#include "asset_loader/include/asset_handle.h"

namespace mono_forge 
{

// A empty texture asset handle for testing purposes
class EmptyTextureAssetHandle : public asset_loader::AssetHandle<EmptyTextureAssetHandle> {};

// A triangle mesh asset handle
class TriangleMeshAssetHandle : public asset_loader::AssetHandle<TriangleMeshAssetHandle> {};

// A box mesh asset handle
class BoxMeshAssetHandle : public asset_loader::AssetHandle<BoxMeshAssetHandle> {};
constexpr const char* BOX_MESH_ASSET_PATH = "../resources/mono_forge/box.mfm";
constexpr DirectX::XMFLOAT4 BOX_DEFAULT_BASE_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr float BOX_DEFAULT_ROUGHNESS_VALUE = 1.0f;
constexpr float BOX_DEFAULT_METALNESS_VALUE = 0.0f;
constexpr DirectX::XMFLOAT4 BOX_DEFAULT_EMISSION_COLOR = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

// A marble bust mesh asset handle for testing purposes
class MarbleBustMeshAssetHandle : public asset_loader::AssetHandle<MarbleBustMeshAssetHandle> {};
constexpr DirectX::XMFLOAT4 MARBLE_BUST_DEFAULT_BASE_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr DirectX::XMFLOAT4 MARBLE_BUST_DEFAULT_EMISSION_COLOR = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr float MARBLE_BUST_DEFAULT_ROUGHNESS_VALUE = 1.0f;
constexpr float MARBLE_BUST_DEFAULT_METALNESS_VALUE = 0.0f;
constexpr const char* MARBLE_BUST_MODEL_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_1k.mfm";

// A marble bust albedo texture asset handle for testing purposes
class MarbleBustAlbedoTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustAlbedoTextureAssetHandle> {};
constexpr const char* MARBLE_BUST_ALBEDO_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_diff_1k.png";

// A marble bust normal texture asset handle for testing purposes
class MarbleBustNormalTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustNormalTextureAssetHandle> {};
constexpr const char* MARBLE_BUST_NORMAL_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_nor_dx_1k.png";

// A marble bust ambient occlusion texture asset handle for testing purposes
class MarbleBustAOTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustAOTextureAssetHandle> {};
constexpr const char* MARBLE_BUST_AO_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_ao_1k.png";

// A marble bust roughness texture asset handle for testing purposes
class MarbleBustRoughnessTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustRoughnessTextureAssetHandle> {};
constexpr const char* MARBLE_BUST_ROUGHNESS_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_rough_1k.png";

// A floor mesh asset handle for testing purposes
class FloorMeshAssetHandle : public asset_loader::AssetHandle<FloorMeshAssetHandle> {};
constexpr DirectX::XMFLOAT4 FLOOR_DEFAULT_BASE_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr DirectX::XMFLOAT4 FLOOR_DEFAULT_EMISSION_COLOR = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr const char* FLOOR_MODEL_PATH = "../resources/render_graph_test/floor/floor.mfm";

} // namespace mono_forge