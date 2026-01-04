// geometry_pass_phong.hlsli

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

// Specular source constants
static const int SPECULAR_SOURCE_NONE = 0;
static const int SPECULAR_SOURCE_TEXTURE = 1;

// Roughness source constants
static const int ROUGHNESS_SOURCE_VALUE = 0;
static const int ROUGHNESS_SOURCE_TEXTURE = 1;

// Roughness default value
static const float ROUGHNESS_DEFAULT = 0.5f;

// Metalness source constants
static const int METALNESS_SOURCE_VALUE = 0;
static const int METALNESS_SOURCE_TEXTURE = 1;

// Metalness default value
static const float METALNESS_DEFAULT = 0.0f;

// Emission source constants
static const int EMISSION_SOURCE_COLOR = 0;
static const int EMISSION_SOURCE_TEXTURE = 1;

cbuffer MaterialBuffer : register(b2)
{
    // Unique material identifier
    uint material_id;

    float3 _material_buffer_padding0; // Padding for 16-byte alignment

    // Base color of the material
    float4 base_color;

    // Albedo source
    uint albedo_source;

    float3 _material_buffer_padding1; // Padding for 16-byte alignment

    // Normal source
    uint normal_source;

    float3 _material_buffer_padding2; // Padding for 16-byte alignment

    // AO source
    uint ao_source;

    float3 _material_buffer_padding3; // Padding for 16-byte alignment

    // Specular source
    uint specular_source;

    float3 _material_buffer_padding5; // Padding for 16-byte alignment

    // Roughness source
    uint roughness_source;

    float3 _material_buffer_padding6; // Padding for 16-byte alignment

    float roughness_value;

    float3 _material_buffer_padding9; // Padding for 16-byte alignment

    // Metalness source
    uint metalness_source;

    float3 _material_buffer_padding7; // Padding for 16-byte alignment

    // Metalness value
    float metalness_value;

    float3 _material_buffer_padding8; // Padding for 16-byte alignment

    // Emission source
    uint emission_source;

    float3 _material_buffer_padding4; // Padding for 16-byte alignment

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

// Specular Texture
Texture2D<float4> texture_specular : register(t3, space0);

// Roughness Texture
Texture2D<float> texture_roughness : register(t4, space0);

// Metalness Texture
Texture2D<float> texture_metalness : register(t5, space0);

// Emission Texture
Texture2D<float4> texture_emission : register(t6, space0);

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

// Function to create specular color
float4 CreateSpecular(float2 uv, float4 albedo, float metalness)
{
    // Sample specular from texture
    float4 sample_texture_specular = texture_specular.Sample(sampler_liner, uv);

    // Specular default color based on metalness
    float3 specular_color = lerp(float3(1,1,1), albedo, metalness);

    // Determine final specular color
    return lerp(float4(specular_color, 1.0f), sample_texture_specular, specular_source);
}

// Function to create roughness value
float CreateRoughness(float2 uv)
{
    // Sample roughness from texture
    float sample_texture_roughness = texture_roughness.Sample(sampler_liner, uv).r;

    // Determine final roughness
    return lerp(roughness_value, sample_texture_roughness, roughness_source);
}

// Function to create metalness value
float CreateMetalness(float2 uv)
{
    // Sample metalness from texture
    float sample_texture_metalness = texture_metalness.Sample(sampler_liner, uv).r;

    // Determine final metalness
    return lerp(metalness_value, sample_texture_metalness, metalness_source);
}

// Function to create emission color
float4 CreateEmission(float2 uv)
{
    // Sample emission from texture
    float4 sample_texture_emission = texture_emission.Sample(sampler_liner, uv);

    // Determine final emission color
    return lerp(emission_color, sample_texture_emission, emission_source);
}