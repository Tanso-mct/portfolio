#pragma once

#include <DirectXMath.h>

#include "render_graph/include/dll_config.h"
#include "render_graph/include/material.h"
#include "render_graph/include/resource_handle.h"
#include "render_graph/include/hlsl_helper.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

// Material type handle for Lambert material
class RENDER_GRAPH_DLL LambertMaterialTypeHandle : public MaterialTypeHandle<LambertMaterialTypeHandle> {};

// Lambert material class
class RENDER_GRAPH_DLL LambertMaterial : 
    public Material
{
public:
    LambertMaterial(ResourceAdder& resource_adder, ResourceEraser& resource_eraser);
    ~LambertMaterial() override;

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

    // Emission source constants
    inline static const int EMISSION_SOURCE_NONE = 0;
    inline static const int EMISSION_SOURCE_TEXTURE = 1;

    // Material Constant Buffer
    // cbuffer MaterialBuffer : register(b2)
    struct MaterialBuffer
    {
        // Unique material identifier
        uint material_id = 0;

        float _padding0[3]; // Padding for 16-byte alignment

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

        // Emission source
        uint emission_source = EMISSION_SOURCE_NONE;

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
            emission_source(EMISSION_SOURCE_NONE),
            emission_color(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
        {
        }

        ~SetupParam() override = default;

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

    // Get emission texture handle
    const render_graph::ResourceHandle* GetEmissionTextureHandle() const { return emission_texture_handle_; }

    // Set emission texture handle
    void SetEmissionTextureHandle(const render_graph::ResourceHandle* handle);

private:
    // Resource adder reference
    ResourceAdder& resource_adder_;

    // Resource eraser reference
    ResourceEraser& resource_eraser_;

    // Lambert material data
    MaterialBuffer buffer_ = MaterialBuffer();

    // Material buffer handle
    render_graph::ResourceHandle material_buffer_handle_ = render_graph::ResourceHandle();

    // Albedo texture handle
    const render_graph::ResourceHandle* albedo_texture_handle_ = nullptr;

    // Normal texture handle
    const render_graph::ResourceHandle* normal_texture_handle_ = nullptr;

    // AO texture handle
    const render_graph::ResourceHandle* ao_texture_handle_ = nullptr;

    // Emission texture handle
    const render_graph::ResourceHandle* emission_texture_handle_ = nullptr;
};

} // namespace render_graph