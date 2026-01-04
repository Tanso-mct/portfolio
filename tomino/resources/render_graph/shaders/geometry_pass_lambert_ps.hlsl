// geometry_pass_lambert_ps.hlsl

#include "geometry_pass.hlsli"
#include "geometry_pass_lambert.hlsli"

// Pixel Shader Main Function
PS_OUT main(VS_OUT input)
{
    PS_OUT output;

    // Create albedo considering albedo source
    output.albedo = CreateAlbedo(input.uv);

    // Create normal considering normal source
    output.normal = float4(input.normal, 1.0f);

    // Create AO considering AO source
    output.ao = CreateAO(input.uv);

    // Create emission considering emission source
    output.emission = CreateEmission(input.uv);

    // Store material ID in the output mask
    output.mask_material = material_id;

    return output;
}