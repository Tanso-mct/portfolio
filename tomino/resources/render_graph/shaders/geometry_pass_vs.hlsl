// geometry_pass_lambert_vs.hlsl

#include "geometry_pass.hlsli"
#include "geometry_pass_lambert.hlsli"
#include "vs_util.hlsli"

// Vertex Shader Main Function
VS_OUT main(VS_IN input)
{
    // Create output structure
    VS_OUT output;

    // Transform position to clip space
    output.pos = TransformPosition(input.pos, world_matrix, view_proj_matrix);

    // Pass through texture coordinates
    output.uv = input.uv;

    // Transform normal to world space
    output.normal = TransformNormal(input.normal, world_inverse_transpose_matrix);

    // Transform tangent to world space
    output.tangent = TransformTangent(input.tangent, world_matrix);

    return output;
}