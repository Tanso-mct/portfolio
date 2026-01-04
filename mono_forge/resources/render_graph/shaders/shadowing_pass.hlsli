// shadowing_pass.hlsli

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
};

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

// World Matrix Constant Buffer
cbuffer WorldMatrixBuffer : register(b2)
{
    matrix world_matrix;
};