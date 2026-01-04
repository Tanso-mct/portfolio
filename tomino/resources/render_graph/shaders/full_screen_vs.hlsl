#include "full_screen.hlsli"

// Vertex Shader Main Function
VS_OUT main(VS_IN input)
{
    // Create output structure
    VS_OUT output;

    output.pos = float4(input.pos, 1.0f);
    output.uv = float2((input.pos.x + 1.0) * 0.5, 1.0 - (input.pos.y + 1.0) * 0.5);

    return output;
}