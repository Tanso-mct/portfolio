#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/asset_loader_set.h"

#include "utility_header/file_loader.h"

#include "mono_asset_extension/include/asset_source_creator.h"

namespace mono_asset_extension
{

void AssetLoaderSet::StoreToMap(AssetRequestComponent::LoaderMap& loader_map)
{
    // Create and add mesh asset loader
    std::unique_ptr<asset_loader::AssetLoader> mesh_loader = std::make_unique<MeshAssetLoader>();
    loader_map.emplace(mesh_loader->GetID(), std::move(mesh_loader));

    // Create and add texture asset loader
    std::unique_ptr<asset_loader::AssetLoader> texture_loader = std::make_unique<TextureAssetLoader>();
    loader_map.emplace(texture_loader->GetID(), std::move(texture_loader));
}

std::unique_ptr<asset_loader::AssetSource> AssetLoaderSet::ExtractAssetSourceToLoad(std::wstring_view file_path)
{
    // Create asset source creator map
    // Key: file extension, Value: asset source creator
    std::unordered_map<std::string_view, std::unique_ptr<AssetSourceCreator>> creator_map;

    creator_map[".mfm"] = std::make_unique<MeshAssetSourceCreator>();
    creator_map[".png"] = std::make_unique<TextureAssetSourceCreator>();

	// Convert file path to string
	std::string file_path_str(file_path.begin(), file_path.end());

    // Determine asset type by file extension
    std::string_view file_extension 
        = utility_header::GetFileExtension(file_path_str);

    // Find the appropriate asset source creator
    auto it = creator_map.find(file_extension);
    if (it == creator_map.end())
        assert(false && "No asset source creator found for the given file extension");

    // Create asset source using the found creator
    return it->second->CreateAssetSource(file_path, service_proxy_manager_);
}

} // namespace mono_asset_extension