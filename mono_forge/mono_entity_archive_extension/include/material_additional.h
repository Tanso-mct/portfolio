#pragma once

#include "material_editor/include/setup_param.h"
#include "asset_loader/include/asset_handle.h"

namespace mono_entity_archive_extension
{

class LambertMaterialAdditionalSetupParam : 
    public material_editor::AdditionalSetupParam
{
public:
    LambertMaterialAdditionalSetupParam(
        asset_loader::AssetHandleID albedo_texture_asset_id,
        asset_loader::AssetHandleID normal_texture_asset_id,
        asset_loader::AssetHandleID ao_texture_asset_id,
        asset_loader::AssetHandleID emission_texture_asset_id) :
        albedo_texture_asset_id(albedo_texture_asset_id),
        normal_texture_asset_id(normal_texture_asset_id),
        ao_texture_asset_id(ao_texture_asset_id),
        emission_texture_asset_id(emission_texture_asset_id)
    {
    }
    ~LambertMaterialAdditionalSetupParam() override = default;

    // Clone the additional setup param
    std::unique_ptr<material_editor::AdditionalSetupParam> Clone() const override
    {
        return std::make_unique<LambertMaterialAdditionalSetupParam>(*this);
    }

    // Get the albedo texture asset ID
    const asset_loader::AssetHandleID& GetAlbedoTextureAssetID() const { return albedo_texture_asset_id; }

    // Set the albedo texture asset ID
    void SetAlbedoTextureAssetID(const asset_loader::AssetHandleID& asset_id) { albedo_texture_asset_id = asset_id; }

    // Get the normal texture asset ID
    const asset_loader::AssetHandleID& GetNormalTextureAssetID() const { return normal_texture_asset_id; }

    // Set the normal texture asset ID
    void SetNormalTextureAssetID(const asset_loader::AssetHandleID& asset_id) { normal_texture_asset_id = asset_id; }

    // Get the AO texture asset ID
    const asset_loader::AssetHandleID& GetAOTextureAssetID() const { return ao_texture_asset_id; }

    // Set the AO texture asset ID
    void SetAOTextureAssetID(const asset_loader::AssetHandleID& asset_id) { ao_texture_asset_id = asset_id; }

    // Get the emission texture asset ID
    const asset_loader::AssetHandleID& GetEmissionTextureAssetID() const { return emission_texture_asset_id; }

    // Set the emission texture asset ID
    void SetEmissionTextureAssetID(const asset_loader::AssetHandleID& asset_id) { emission_texture_asset_id = asset_id; }

private:
    // Albedo texture asset ID
    asset_loader::AssetHandleID albedo_texture_asset_id = asset_loader::AssetHandleID();

    // Normal texture asset ID
    asset_loader::AssetHandleID normal_texture_asset_id = asset_loader::AssetHandleID();

    // AO texture asset ID
    asset_loader::AssetHandleID ao_texture_asset_id = asset_loader::AssetHandleID();

    // Emission texture asset ID
    asset_loader::AssetHandleID emission_texture_asset_id = asset_loader::AssetHandleID();
};

class PhongMaterialAdditionalSetupParam : 
    public material_editor::AdditionalSetupParam
{
public:
    PhongMaterialAdditionalSetupParam(
        asset_loader::AssetHandleID albedo_texture_asset_id,
        asset_loader::AssetHandleID normal_texture_asset_id,
        asset_loader::AssetHandleID ao_texture_asset_id,
        asset_loader::AssetHandleID specular_texture_asset_id,
        asset_loader::AssetHandleID metalness_texture_asset_id,
        asset_loader::AssetHandleID roughness_texture_asset_id,
        asset_loader::AssetHandleID emission_texture_asset_id) :
        albedo_texture_asset_id(albedo_texture_asset_id),
        normal_texture_asset_id(normal_texture_asset_id),
        ao_texture_asset_id(ao_texture_asset_id),
        specular_texture_asset_id(specular_texture_asset_id),
        metalness_texture_asset_id(metalness_texture_asset_id),
        roughness_texture_asset_id(roughness_texture_asset_id),
        emission_texture_asset_id(emission_texture_asset_id)
    {
    }
    ~PhongMaterialAdditionalSetupParam() override = default;

    // Clone the additional setup param
    std::unique_ptr<material_editor::AdditionalSetupParam> Clone() const override
    {
        return std::make_unique<PhongMaterialAdditionalSetupParam>(*this);
    }

    // Get the albedo texture asset ID
    const asset_loader::AssetHandleID& GetAlbedoTextureAssetID() const { return albedo_texture_asset_id; }

    // Set the albedo texture asset ID
    void SetAlbedoTextureAssetID(const asset_loader::AssetHandleID& asset_id) { albedo_texture_asset_id = asset_id; }

    // Get the normal texture asset ID
    const asset_loader::AssetHandleID& GetNormalTextureAssetID() const { return normal_texture_asset_id; }

    // Set the normal texture asset ID
    void SetNormalTextureAssetID(const asset_loader::AssetHandleID& asset_id) { normal_texture_asset_id = asset_id; }

    // Get the AO texture asset ID
    const asset_loader::AssetHandleID& GetAOTextureAssetID() const { return ao_texture_asset_id; }

    // Set the AO texture asset ID
    void SetAOTextureAssetID(const asset_loader::AssetHandleID& asset_id) { ao_texture_asset_id = asset_id; }

    // Get the specular texture asset ID
    const asset_loader::AssetHandleID& GetSpecularTextureAssetID() const { return specular_texture_asset_id; }

    // Set the specular texture asset ID
    void SetSpecularTextureAssetID(const asset_loader::AssetHandleID& asset_id) { specular_texture_asset_id = asset_id; }

    // Get the metalness texture asset ID
    const asset_loader::AssetHandleID& GetMetalnessTextureAssetID() const { return metalness_texture_asset_id; }

    // Set the metalness texture asset ID
    void SetMetalnessTextureAssetID(const asset_loader::AssetHandleID& asset_id) { metalness_texture_asset_id = asset_id; }

    // Get the roughness texture asset ID
    const asset_loader::AssetHandleID& GetRoughnessTextureAssetID() const { return roughness_texture_asset_id; }

    // Set the roughness texture asset ID
    void SetRoughnessTextureAssetID(const asset_loader::AssetHandleID& asset_id) { roughness_texture_asset_id = asset_id; }

    // Get the emission texture asset ID
    const asset_loader::AssetHandleID& GetEmissionTextureAssetID() const { return emission_texture_asset_id; }

    // Set the emission texture asset ID
    void SetEmissionTextureAssetID(const asset_loader::AssetHandleID& asset_id) { emission_texture_asset_id = asset_id; }

private:
    // Albedo texture asset ID
    asset_loader::AssetHandleID albedo_texture_asset_id = asset_loader::AssetHandleID();

    // Normal texture asset ID
    asset_loader::AssetHandleID normal_texture_asset_id = asset_loader::AssetHandleID();

    // AO texture asset ID
    asset_loader::AssetHandleID ao_texture_asset_id = asset_loader::AssetHandleID();

    // Specular texture asset ID
    asset_loader::AssetHandleID specular_texture_asset_id = asset_loader::AssetHandleID();

    // Metalness texture asset ID
    asset_loader::AssetHandleID metalness_texture_asset_id = asset_loader::AssetHandleID();

    // Roughness texture asset ID
    asset_loader::AssetHandleID roughness_texture_asset_id = asset_loader::AssetHandleID();

    // Emission texture asset ID
    asset_loader::AssetHandleID emission_texture_asset_id = asset_loader::AssetHandleID();

};

} // namespace mono_entity_archive_extension