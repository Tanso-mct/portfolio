// geometry_pass_lambert.hlsli

// Albedo source constants
static const int ALBEDO_SOURCE_BASE_COLOR = 0;
static const int ALBEDO_SOURCE_TEXTURE = 1;

// Normal source constants
static const int NORMAL_SOURCE_VERTEX = 0;
static const int NORMAL_SOURCE_TEXTURE = 1;

// AO source constants
static const int AO_SOURCE_NONE = 0;
static const int AO_SOURCE_TEXTURE = 1;

// AO default value
static const float AO_DEFAULT = 1.0f;

// Emission source constants
static const int EMISSION_SOURCE_NONE = 0;
static const int EMISSION_SOURCE_TEXTURE = 1;

// Material Constant Buffer
cbuffer MaterialBuffer : register(b2)
{
    // Unique material identifier
    uint material_id;

    float3 _material_buffer_padding0; // Padding for alignment

    // Base color of the material
    float4 base_color;

    // Albedo source
    uint albedo_source;

    float3 _material_buffer_padding1; // Padding for alignment

    // Normal source
    uint normal_source;

    float3 _material_buffer_padding2; // Padding for alignment

    // AO source
    uint ao_source;

    float3 _material_buffer_padding3; // Padding for alignment

    // Emission source
    uint emission_source;

    float3 _material_buffer_padding4; // Padding for alignment

    // Emission color
    float4 emission_color;
};

// Linear Sampler
SamplerState sampler_liner : register(s0, space0);

// Albedo Texture
Texture2D<float4> texture_albedo : register(t0, space0);

// Normal Texture
Texture2D<float4> texture_normal : register(t1, space0);

// AO Texture
Texture2D<float> texture_ao : register(t2, space0);

// Emission Texture
Texture2D<float4> texture_emission : register(t3, space0);

// Function to create albedo color
float4 CreateAlbedo(float2 uv)
{
    // Sample albedo texture color
    float4 sample_texture_albedo = texture_albedo.Sample(sampler_liner, uv);

    // Determine final albedo color
    return lerp(base_color, sample_texture_albedo, albedo_source);
}

// Function to create normal
float3 CreateNormal(float2 uv, float3 normal, float3 tangent)
{
    // Sample normal from texture
    float4 sample_texture_normal = texture_normal.Sample(sampler_liner, uv) * 2.0f - 1.0f; // Transform from [0,1] to [-1,1]

    // Construct TBN matrix
    float3 N = normalize(normal);
    float3 T = normalize(tangent);
    float3 B = normalize(cross(N, T)); //POINT
    float3x3 TBN = float3x3(T, B, N);

    // Transform texture normal to world space
    float3 texture_world_normal = normalize(mul(sample_texture_normal.xyz, TBN));

    // Determine final normal
    return normalize(lerp(normal, texture_world_normal, normal_source));
}

// Function to create AO value
float CreateAO(float2 uv)
{
    // Sample AO from texture
    float sample_texture_ao = texture_ao.Sample(sampler_liner, uv).r;

    // Determine final AO
    return lerp(AO_DEFAULT, sample_texture_ao, ao_source);
}

// Function to create emission color
float4 CreateEmission(float2 uv)
{
    // Sample emission from texture
    float4 sample_texture_emission = texture_emission.Sample(sampler_liner, uv);

    // Determine final emission color
    return lerp(emission_color, sample_texture_emission, emission_source);
}