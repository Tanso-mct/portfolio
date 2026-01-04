// geometry_pass.hlsli

// Vertex Shader Input Structure
struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Vertex Shader Output Structure
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 tangent : TEXCOORD2;
};

// Pixel Shader Output Structure
struct PS_OUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float ao : SV_Target2;
    float4 specular : SV_Target3;
    float roughness : SV_Target4;
    float metalness : SV_Target5;
    float4 emission : SV_Target6;
    uint mask_material : SV_Target7;
};

// View-Projection Constant Buffer
cbuffer ViewProjBuffer : register(b0)
{
    matrix view_proj_matrix;
};

// World Constant Buffer
cbuffer WorldBuffer : register(b1)
{
    matrix world_matrix;
    matrix world_inverse_transpose_matrix;
};