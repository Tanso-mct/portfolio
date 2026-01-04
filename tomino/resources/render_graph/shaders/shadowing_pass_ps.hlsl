// shadowing_pass_ps.hlsl

#include "shadowing_pass.hlsli"

// Pixel Shader Main Function
float main(VS_OUT input) : SV_DEPTH
{
    // Output depth value
    return input.pos.z / input.pos.w;
}