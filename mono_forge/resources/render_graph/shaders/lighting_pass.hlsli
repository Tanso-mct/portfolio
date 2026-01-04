// lighting_pass.hlsli

// Inverse View-Projection Constant Buffer
cbuffer InverseViewProjBuffer : register(b0)
{
    matrix inv_view_proj_matrix;
};

// Light Configuration Constant Buffer
cbuffer LightConfigBuffer : register(b1)
{
    // Number of lights in the scene
    uint num_lights;

    float3 _light_config_buffer_padding0; // Padding for alignment

    // Maximum number of lights supported
    uint max_lights;

    float3 _light_config_buffer_padding0b; // Padding for alignment

    // Screen size (width, height)
    float2 screen_size;

    float2 _light_config_buffer_padding1; // Padding for alignment

    // Shadow intensity
    float shadow_intensity;

    float3 _light_config_buffer_padding1b; // Padding for alignment

    // Camera world matrix
    matrix camera_world_matrix;

    // Lambert material ID
    uint lambert_material_id;

    float3 _light_config_buffer_padding2; // Padding for alignment

    // Phong material ID
    uint phong_material_id;

    float3 _light_config_buffer_padding2b; // Padding for alignment

    // Directional light ID
    uint directional_light_id;

    float3 _light_config_buffer_padding3; // Padding for alignment

    // Ambient light ID
    uint ambient_light_id;

    float3 _light_config_buffer_padding4; // Padding for alignment

    // Point light ID
    uint point_light_id;

    float3 _light_config_buffer_padding5; // Padding for alignment
};

// Maximum number of lights supported
static const uint LIGHT_MAX_COUNT = 10;

// Default direction of the light, pointing along positive Z-axis
static const float3 DEFAULT_DIRECTION = float3(0.0f, 0.0f, 1.0f);

// Structure for light buffer
struct LightBuffer
{
    // Type of the light (e.g., directional, point, spot)
    uint light_type;

    float3 _light_buffer_padding0; // Padding for alignment

    // World matrix of the light
    matrix light_world_matrix;

    // Color of the light
    float4 light_color;

    // Intensity of the light
    float light_intensity;

    float3 _light_buffer_padding1; // Padding for alignment

    // Range of the light
    float range;

    float3 _light_buffer_padding2; // Padding for alignment
};

// Linear Sampler
SamplerState sampler_liner : register(s0, space0);

// Structured buffer for lights
StructuredBuffer<LightBuffer> lights : register(t0, space0);

// G-Buffer Textures
Texture2D<float4> albedo_texture : register(t1, space0);
Texture2D<float4> normal_texture : register(t2, space0);
Texture2D<float> metalness_texture : register(t3, space0);
Texture2D<float> roughness_texture : register(t4, space0);
Texture2D<float4> specular_texture : register(t5, space0);
Texture2D<float> ao_texture : register(t6, space0);
Texture2D<float4> emission_texture : register(t7, space0);
Texture2D<uint> mask_material_texture : register(t8, space0);
Texture2D<float> mask_shadow_texture : register(t9, space0);
Texture2D<float> depth_texture : register(t10, space0);

// Pixel Shader Output Structure
struct PS_OUT
{
    float4 color : SV_Target0;
};