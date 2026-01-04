// shadowing_pass_vs.hlsl

#include "shadowing_pass.hlsli"
#include "vs_util.hlsli"

// Vertex Shader Main Function
VS_OUT main(VS_IN input)
{
    VS_OUT output;

    // Compute Light View-Projection Matrix
    matrix light_view_proj_matrix = mul(light_view_matrix, light_proj_matrix);

    // Transform position to light's clip space
    output.pos = TransformPosition(input.pos, world_matrix, light_view_proj_matrix);
    output.pos.x = -output.pos.x; // Invert X for left-handed to right-handed conversion

    return output;
}