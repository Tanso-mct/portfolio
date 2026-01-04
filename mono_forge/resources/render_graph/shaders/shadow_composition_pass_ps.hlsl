// shadow_composition_pass_ps.hlsl

#include "full_screen.hlsli"
#include "shadow_composition_pass.hlsli"
#include "ps_util.hlsli"

// Pixel Shader Main Function
float main(VS_OUT input) : SV_Target
{
    // Calculate pixel coordinates
    uint2 pixel_coord = uint2(input.uv * screen_size);

    // Sample depth from depth texture
    float depth = depth_texture.Sample(sampler_point, input.uv).r;

    // Reconstruct world position
    float3 world_pos = ReconstructWorldPos(pixel_coord, depth, inv_view_proj_matrix, screen_size);

    // Combine light view and projection matrices
    matrix light_view_proj_matrix = mul(light_view_matrix, light_proj_matrix);

    // Transform world position to light's clip space
    float4 light_clip_pos = mul(float4(world_pos, 1.0f), light_view_proj_matrix);
    
    // Perform perspective divide
    float3 light_ndc = light_clip_pos.xyz / light_clip_pos.w;

    // Calculate shadow UV coordinates
    float2 shadow_uv = light_ndc.xy * 0.5f + 0.5f;
    shadow_uv = 1.0f - shadow_uv; // Flip
    
    // Outside of shadow map -> treat as lit
    if (shadow_uv.x < 0.0f || shadow_uv.x > 1.0f || shadow_uv.y < 0.0f || shadow_uv.y > 1.0f)
        return 1.0f;

    // Light direction calculation for slope scaled bias
    float3x3 light_rotation = (float3x3)transpose(light_view_matrix); // Transpose for local to world
    float3 light_direction = -light_rotation[2];

    // Sample normal from normal texture
    float3 surface_normal = normalize(normal_texture.Sample(sampler_point, input.uv).xyz);

    // Calculate slope scaled bias
    float ndotl = saturate(dot(surface_normal, light_direction));

    float slope_bias = shadow_bias + slope_scaled_bias * pow(1.0f - ndotl, slope_bias_exponent);

    // Sample shadow map with comparison
    float shadow = shadow_map_texture.SampleCmp(sampler_cmp, shadow_uv, light_ndc.z - slope_bias);

    return shadow; // Return shadow factor
}