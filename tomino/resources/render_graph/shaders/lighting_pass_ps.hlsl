// lighting_pass_ps.hlsl

#include "lighting_pass.hlsli"
#include "full_screen.hlsli"
#include "ps_util.hlsli"
#include "light_util.hlsli"

PS_OUT main(VS_OUT input)
{
    PS_OUT output;

    // Calculate pixel coordinates
    uint2 pixel_coord = uint2(input.uv * screen_size);

    // Sample G-Buffer textures
    float4 albedo = albedo_texture.Sample(sampler_liner, input.uv);
    float4 normal = normal_texture.Sample(sampler_liner, input.uv);
    float ao = ao_texture.Sample(sampler_liner, input.uv);
    float4 specular = specular_texture.Sample(sampler_liner, input.uv);
    float roughness = roughness_texture.Sample(sampler_liner, input.uv);
    float metalness = metalness_texture.Sample(sampler_liner, input.uv);
    float4 emission = emission_texture.Sample(sampler_liner, input.uv);
    uint mask_material = mask_material_texture.Load(int3(pixel_coord, 0));
    float depth = depth_texture.Sample(sampler_liner, input.uv).r;

    // Reconstruct world position
    float3 world_pos = ReconstructWorldPos(pixel_coord, depth, inv_view_proj_matrix, screen_size);

    // Iterate through all lights and accumulate lighting
    for (uint i = 0; i < num_lights; ++i)
    {
        // Fetch light data
        LightBuffer light = lights[i];

        // Apply lighting based on material type
        if (mask_material == lambert_material_id) // Lambert
        {
            if (light.light_type == directional_light_id) // Directional Light
            {
                output.color.rgb += CreateLambertColor(
                    albedo.rgb, normal.xyz, ao,
                    TransformDirection(DEFAULT_DIRECTION, (float3x3)light.light_world_matrix),
                    light.light_color.rgb, light.light_intensity);
            }
            else if (light.light_type == ambient_light_id) // Ambient Light
            {
                output.color.rgb += CreateLambertColor(
                    albedo.rgb, ao, light.light_color.rgb, light.light_intensity);
            }
            else if (light.light_type == point_light_id) // Point Light
            {
                output.color.rgb += CreateLambertColor(
                    albedo.rgb, normal.xyz, ao, world_pos,
                    light.light_world_matrix[3].xyz, light.light_color.rgb, light.light_intensity, light.range);
            }

            // Emission contribution
            output.color.rgb += emission.rgb;

            // Set alpha to 1.0 (opaque)
            output.color.a = 1.0f;
        }
        else if (mask_material == phong_material_id) // Phong
        {
            if (light.light_type == directional_light_id) // Directional Light
            {
                output.color.rgb += CreatePhongColor(
                    albedo.rgb, normal.xyz, ao, specular.rgb, roughness,
                    world_pos, camera_world_matrix[3].xyz,
                    TransformDirection(DEFAULT_DIRECTION, (float3x3)light.light_world_matrix),
                    light.light_color.rgb, light.light_intensity);
            }
            else if (light.light_type == ambient_light_id) // Ambient Light
            {
                output.color.rgb += CreateLambertColor(
                    albedo.rgb, ao, light.light_color.rgb, light.light_intensity);
            }
            else if (light.light_type == point_light_id) // Point Light
            {
                output.color.rgb += CreatePhongColor(
                    albedo.rgb, normal.xyz, ao, specular.rgb, roughness,
                    world_pos, camera_world_matrix[3].xyz,
                    light.light_world_matrix[3].xyz, light.light_color.rgb, light.light_intensity, light.range);
            }

            // Emission contribution
            output.color.rgb += emission.rgb;

            // Set alpha to 1.0 (opaque)
            output.color.a = 1.0f;
        }
    }

    // Sample shadow from mask shadow texture
    float shadow = mask_shadow_texture.Sample(sampler_liner, input.uv).r;

    // Apply shadow intensity
    output.color.rgb *= lerp(1.0f, shadow_intensity, 1.0f - shadow);

    return output;
}