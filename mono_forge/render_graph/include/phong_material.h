#pragma once

#include <DirectXMath.h>

#include "render_graph/include/dll_config.h"
#include "render_graph/include/material.h"
#include "render_graph/include/resource_handle.h"
#include "render_graph/include/hlsl_helper.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

// Material type handle
class RENDER_GRAPH_DLL PhongMaterialTypeHandle : public MaterialTypeHandle<PhongMaterialTypeHandle> {};

// Material class
class RENDER_GRAPH_DLL PhongMaterial : 
    public Material
{
public:
    PhongMaterial(ResourceAdder& resource_adder, ResourceEraser& resource_eraser);
    ~PhongMaterial() override;

    /*******************************************************************************************************************
     * HLSL Corresponding Definitions
    /******************************************************************************************************************/

    // Albedo source constants
    inline static const int ALBEDO_SOURCE_BASE_COLOR = 0;
    inline static const int ALBEDO_SOURCE_TEXTURE = 1;

    // Normal source constants
    inline static const int NORMAL_SOURCE_VERTEX = 0;
    inline static const int NORMAL_SOURCE_TEXTURE = 1;

    // AO source constants
    inline static const int AO_SOURCE_NONE = 0;
    inline static const int AO_SOURCE_TEXTURE = 1;

    // AO default value
    inline static const float AO_DEFAULT = 1.0f;

    // Specular source constants
    inline static const int SPECULAR_SOURCE_NONE = 0;
    inline static const int SPECULAR_SOURCE_TEXTURE = 1;

    // Roughness source constants
    inline static const int ROUGHNESS_SOURCE_VALUE = 0;
    inline static const int ROUGHNESS_SOURCE_TEXTURE = 1;

    // Roughness default value
    inline static const float ROUGHNESS_DEFAULT = 0.5f;

    // Metalness source constants
    inline static const int METALNESS_SOURCE_VALUE = 0;
    inline static const int METALNESS_SOURCE_TEXTURE = 1;

    // Metalness default value
    inline static const float METALNESS_DEFAULT = 0.0f;

    // Emission source constants
    inline static const int EMISSION_SOURCE_COLOR = 0;
    inline static const int EMISSION_SOURCE_TEXTURE = 1;

    struct MaterialBuffer
    {
        // Unique material identifier
        uint material_id = 0;

        float3 _padding0; // Padding for 16-byte alignment

        // Base color of the material
        float4 base_color = float4(1.0f, 1.0f, 1.0f, 1.0f);

        // Albedo source
        uint albedo_source = ALBEDO_SOURCE_BASE_COLOR;

        float3 _padding1; // Padding for 16-byte alignment

        // Normal source
        uint normal_source = NORMAL_SOURCE_VERTEX;

        float3 _padding2; // Padding for 16-byte alignment

        // AO source
        uint ao_source = AO_SOURCE_NONE;

        float3 _padding3; // Padding for 16-byte alignment

        // Specular source
        uint specular_source = SPECULAR_SOURCE_NONE;

        float3 _padding5; // Padding for 16-byte alignment

        // Roughness source
        uint roughness_source = SPECULAR_SOURCE_NONE;

        float3 _padding6; // Padding for 16-byte alignment

        float roughness_value = ROUGHNESS_DEFAULT;

        float3 _padding9; // Padding for 16-byte alignment

        // Metalness source
        uint metalness_source = METALNESS_SOURCE_VALUE;

        float3 _padding7; // Padding for 16-byte alignment

        // Metalness value
        float metalness_value = METALNESS_DEFAULT;

        float3 _padding8; // Padding for 16-byte alignment

        // Emission source
        uint emission_source = EMISSION_SOURCE_COLOR;

        float3 _padding4; // Padding for 16-byte alignment

        // Emission color
        float4 emission_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    /*******************************************************************************************************************
     * HLSL Corresponding Definitions End
    /******************************************************************************************************************/

    // SetupParam class for Lambert material
    class SetupParam : 
        public Material::SetupParam
    {
    public:
        SetupParam() :
            debug_name_prefix(L"Unnamed"),
            base_color(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
            albedo_source(ALBEDO_SOURCE_BASE_COLOR),
            normal_source(NORMAL_SOURCE_VERTEX),
            ao_source(AO_SOURCE_NONE),
            specular_source(SPECULAR_SOURCE_NONE),
            roughness_source(ROUGHNESS_SOURCE_VALUE),
            roughness_value(ROUGHNESS_DEFAULT),
            metalness_source(METALNESS_SOURCE_VALUE),
            metalness_value(METALNESS_DEFAULT),
            emission_source(EMISSION_SOURCE_COLOR),
            emission_color(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
        {
        }

        ~SetupParam() override = default;

        std::unique_ptr<Material::SetupParam> Clone() const override
        {
            return std::make_unique<SetupParam>(*this);
        }

        // Debug name prefix
        std::wstring debug_name_prefix;

        // Base color of the material
        DirectX::XMFLOAT4 base_color;

        // Albedo source selector
        uint32_t albedo_source;

        // Normal source selector
        uint32_t normal_source;

        // AO source selector
        uint32_t ao_source;

        // Specular source selector
        uint32_t specular_source;

        // Roughness source selector
        uint32_t roughness_source;

        // Roughness value
        float roughness_value;

        // Metalness source selector
        uint32_t metalness_source;

        // Metalness value
        float metalness_value;

        // Emission source selector
        uint32_t emission_source;

        // Emission color
        DirectX::XMFLOAT4 emission_color;

        // Albedo texture handle
        const render_graph::ResourceHandle* albedo_texture_handle = nullptr;

        // Normal texture handle
        const render_graph::ResourceHandle* normal_texture_handle = nullptr;

        // AO texture handle
        const render_graph::ResourceHandle* ao_texture_handle = nullptr;

        // Specular texture handle
        const render_graph::ResourceHandle* specular_texture_handle = nullptr;

        // Roughness texture handle
        const render_graph::ResourceHandle* roughness_texture_handle = nullptr;

        // Metalness texture handle
        const render_graph::ResourceHandle* metalness_texture_handle = nullptr;

        // Emission texture handle
        const render_graph::ResourceHandle* emission_texture_handle = nullptr;
    };

    bool Setup(Material::SetupParam& param) override;
    bool Apply(const Material::SetupParam& param) override;
    const ResourceHandle* GetBufferHandle() const override;
    MaterialTypeHandleID GetMaterialTypeHandleID() const override;
    void DeclareResources(RenderPassBuilder& builder) const override;
    const void* GetBufferData(uint32_t& size) const override;

    // Get base color
    const DirectX::XMFLOAT4& GetBaseColor() const { return buffer_.base_color; }

    // Set base color
    void SetBaseColor(const DirectX::XMFLOAT4& color) { buffer_.base_color = color; }

    // Get albedo source
    int GetAlbedoSource() const { return buffer_.albedo_source; }

    // Set albedo source
    void SetAlbedoSource(int source) { buffer_.albedo_source = source; }

    // Get normal source
    int GetNormalSource() const { return buffer_.normal_source; }

    // Set normal source
    void SetNormalSource(int source) { buffer_.normal_source = source; }

    // Get ao source
    int GetAOSource() const { return buffer_.ao_source; }

    // Set ao source
    void SetAOSource(int source) { buffer_.ao_source = source; }

    // Get specular source
    int GetSpecularSource() const { return buffer_.specular_source; }

    // Set specular source
    void SetSpecularSource(int source) { buffer_.specular_source = source; }

    // Get roughness source
    int GetRoughnessSource() const { return buffer_.roughness_source; }

    // Set roughness source
    void SetRoughnessSource(int source) { buffer_.roughness_source = source; }

    // Get roughness value
    float GetRoughnessValue() const { return buffer_.roughness_value; }

    // Set roughness value
    void SetRoughnessValue(float value) { buffer_.roughness_value = value; }

    // Get metalness source
    int GetMetalnessSource() const { return buffer_.metalness_source; }

    // Set metalness source
    void SetMetalnessSource(int source) { buffer_.metalness_source = source; }

    // Get metalness value
    float GetMetalnessValue() const { return buffer_.metalness_value; }

    // Set metalness value
    void SetMetalnessValue(float value) { buffer_.metalness_value = value; }

    // Get emission source
    int GetEmissionSource() const { return buffer_.emission_source; }

    // Set emission source
    void SetEmissionSource(int source) { buffer_.emission_source = source; }

    // Get emission color
    const DirectX::XMFLOAT4& GetEmissionColor() const { return buffer_.emission_color; }

    // Set emission color
    void SetEmissionColor(const DirectX::XMFLOAT4& color) { buffer_.emission_color = color; }

    // Get albedo texture handle
    const render_graph::ResourceHandle* GetAlbedoTextureHandle() const { return albedo_texture_handle_; }

    // Set albedo texture handle
    void SetAlbedoTextureHandle(const render_graph::ResourceHandle* handle);

    // Get normal texture handle
    const render_graph::ResourceHandle* GetNormalTextureHandle() const { return normal_texture_handle_; }

    // Set normal texture handle
    void SetNormalTextureHandle(const render_graph::ResourceHandle* handle);

    // Get ao texture handle
    const render_graph::ResourceHandle* GetAOTextureHandle() const { return ao_texture_handle_; }

    // Set ao texture handle
    void SetAOTextureHandle(const render_graph::ResourceHandle* handle);

    // Get specular texture handle
    const render_graph::ResourceHandle* GetSpecularTextureHandle() const { return specular_texture_handle_; }

    // Set specular texture handle
    void SetSpecularTextureHandle(const render_graph::ResourceHandle* handle);

    // Get roughness texture handle
    const render_graph::ResourceHandle* GetRoughnessTextureHandle() const { return roughness_texture_handle_; }

    // Set roughness texture handle
    void SetRoughnessTextureHandle(const render_graph::ResourceHandle* handle);

    // Get metalness texture handle
    const render_graph::ResourceHandle* GetMetalnessTextureHandle() const { return metalness_texture_handle_; }

    // Set metalness texture handle
    void SetMetalnessTextureHandle(const render_graph::ResourceHandle* handle);

    // Get emission texture handle
    const render_graph::ResourceHandle* GetEmissionTextureHandle() const { return emission_texture_handle_; }

    // Set emission texture handle
    void SetEmissionTextureHandle(const render_graph::ResourceHandle* handle);

private:
    // Resource adder reference
    ResourceAdder& resource_adder_;

    // Resource eraser reference
    ResourceEraser& resource_eraser_;

    // Material data
    MaterialBuffer buffer_ = MaterialBuffer();

    // Material buffer handle
    render_graph::ResourceHandle material_buffer_handle_ = render_graph::ResourceHandle();

    // Albedo texture handle
    const render_graph::ResourceHandle* albedo_texture_handle_ = nullptr;

    // Normal texture handle
    const render_graph::ResourceHandle* normal_texture_handle_ = nullptr;

    // AO texture handle
    const render_graph::ResourceHandle* ao_texture_handle_ = nullptr;

    // Specular texture handle
    const render_graph::ResourceHandle* specular_texture_handle_ = nullptr;

    // Roughness texture handle
    const render_graph::ResourceHandle* roughness_texture_handle_ = nullptr;

    // Metalness texture handle
    const render_graph::ResourceHandle* metalness_texture_handle_ = nullptr;

    // Emission texture handle
    const render_graph::ResourceHandle* emission_texture_handle_ = nullptr;
};

} // namespace render_graph