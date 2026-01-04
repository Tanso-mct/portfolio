// Full-Screen Shader HLSL Include File

// Vertex Shader Input Structure
struct VS_IN
{
    float3 pos : POSITION;
};

// Vertex Shader Output Structure
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};