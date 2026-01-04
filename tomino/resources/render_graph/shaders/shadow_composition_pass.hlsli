// shadow_composition_pass.hlsli

// Light View Constant Buffer
cbuffer LightViewMatrixBuffer : register(b0)
{
    matrix light_view_matrix;
};

// Light Projection Constant Buffer
cbuffer LightProjMatrixBuffer : register(b1)
{
    matrix light_proj_matrix;
}

// Inverse View-Projection Constant Buffer
cbuffer InverseViewProjBuffer : register(b2)
{
    matrix inv_view_proj_matrix;
};

// Shadow Composition Configuration Constant Buffer
cbuffer ShadowCompositionConfigBuffer : register(b3)
{
    // Screen size (width, height)
    float2 screen_size;

    float2 _shadow_composition_config_buffer_padding0; // Padding for alignment

    // Shadow bias value
    float shadow_bias;

    float3 _shadow_composition_config_buffer_padding1; // Padding for alignment

    // Slope scaled bias value
    float slope_scaled_bias;

    float3 _shadow_composition_config_buffer_padding2; // Padding for alignment

    // Exponent for slope scaled bias calculation
    float slope_bias_exponent;

    float3 _shadow_composition_config_buffer_padding3; // Padding for alignment
};

// Sampler
SamplerState sampler_point : register(s0);
SamplerComparisonState sampler_cmp : register(s1);

// Shadow Map Texture
Texture2D<float> shadow_map_texture : register(t0);

// Depth Texture
Texture2D<float> depth_texture : register(t1);

// Normal Texture
Texture2D<float4> normal_texture : register(t2);