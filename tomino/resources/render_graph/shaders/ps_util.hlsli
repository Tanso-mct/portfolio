// ps_util.hlsli

// Reconstructs world position from screen coordinates and depth
float3 ReconstructWorldPos(
    uint2 pixel_coord, float depth, float4x4 inv_view_proj_matrix, float2 screen_size)
{
    // Convert pixel coordinates and depth to NDC space
    float2 ndc;
    ndc.x = (pixel_coord.x / screen_size.x) * 2.0f - 1.0f;
    ndc.y = 1.0f - (pixel_coord.y / screen_size.y) * 2.0f;

    // Reconstruct clip space position
    float4 clip_pos = float4(ndc.x, ndc.y, depth, 1.0);

    // Transform to world space
    float4 world_pos = mul(clip_pos, inv_view_proj_matrix);
    world_pos /= world_pos.w;

    return world_pos.xyz; // Return world position
}

// Transforms a direction vector using an inverse transpose matrix
float3 TransformDirection(float3 direction, float3x3 world_matrix)
{
    return normalize(mul(direction, world_matrix));
}