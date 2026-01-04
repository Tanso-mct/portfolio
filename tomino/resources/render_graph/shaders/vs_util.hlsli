// vs_util.hlsl

// Transforms a position from model space to clip space
float4 TransformPosition(float3 position, matrix world_matrix, matrix view_proj_matrix)
{
    float4 world_pos = mul(float4(position, 1.0f), world_matrix);
    float4 proj_pos = mul(world_pos, view_proj_matrix);
    return proj_pos; // Position in clip space
}

// Transforms a normal from model space to world space
float3 TransformNormal(float3 normal, matrix world_inverse_transpose)
{
    float3 world_normal = mul(normal, (float3x3)world_inverse_transpose);
    return normalize(world_normal.xyz); // Normal in world space
}

// Transforms a tangent from model space to world space
float3 TransformTangent(float3 tangent, matrix world_matrix)
{
    float4 world_tangent = mul(float4(tangent, 0.0f), world_matrix);
    return normalize(world_tangent.xyz); // Tangent in world space
}