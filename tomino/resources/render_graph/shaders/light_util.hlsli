// light_util.hlsli

/***********************************************************************************************************************
 * Lambert
/**********************************************************************************************************************/

// Creates Lambert color by using light direction
float3 CreateLambertColor(
    float3 albedo, float3 normal, float ao,
    float3 light_dir, float3 light_color, float light_intensity)
{
    // Calculate the dot product between normal and light direction
    float diff = max(dot(normal, -light_dir), 0.0f);

    // Calculate diffuse component
    float3 diffuse = albedo * light_color * diff * light_intensity * ao;

    return diffuse; // Return the diffuse color
}

// Creates Lambert color by using ambient color
float3 CreateLambertColor(float3 albedo, float ao, float3 light_color, float light_intensity)
{
    // Calculate ambient component
    float3 ambient = albedo * light_color * light_intensity * ao;

    return ambient; // Return the ambient color
}

// Create lambert color by using light position
float3 CreateLambertColor(
    float3 albedo, float3 normal, float ao, float3 world_pos,
    float3 light_pos, float3 light_color, float light_intensity, float range)
{
    // Calculate light direction
    float3 light_dir = normalize(world_pos - light_pos);

    // Calculate the dot product between normal and light direction
    float diff = max(dot(normal, -light_dir), 0.0f);

    // Calculate distance to light
    float distance = length(world_pos - light_pos);

    // Calculate attenuation based on distance and range
    float attenuation = saturate(1.0f - (distance / range));

    // Calculate diffuse component
    float3 diffuse = albedo * light_color * diff * light_intensity * attenuation * ao;

    return diffuse; // Return the diffuse color
}

/***********************************************************************************************************************
 * Phong
/**********************************************************************************************************************/

// Creates Phong color by using light direction and view direction
float3 CreatePhongColor(
    float3 albedo, float3 normal, float ao, float3 specular, float roughness,
    float3 world_pos, float3 camera_pos, 
    float3 light_dir, float3 light_color, float light_intensity)
{
    // View direction
    float3 view_dir = normalize(camera_pos - world_pos);

    // Halfway vector
    float3 half_way = normalize(-light_dir + view_dir);

    // Lambert diffuse
    float diff = max(dot(normal, -light_dir), 0.0f);

    // Lambert reflectance
    float3 diffuse = albedo * light_color * diff * light_intensity * ao;

    // Phong specular
    float n_dot_h = max(dot(normal, half_way), 0.0f);
    float shininess = lerp(256.0f, 2.0f, roughness);
    float spec_factor = pow(n_dot_h, shininess);

    // Specular reflectance
    float3 spec_reflectance = specular * spec_factor * light_color * light_intensity * ao * (1.0f - roughness);

    return diffuse + spec_reflectance; // Return combined color
}

// Creates Phong color by using light position
float3 CreatePhongColor(
    float3 albedo, float3 normal, float ao, float3 specular, float roughness,
    float3 world_pos, float3 camera_pos,
    float3 light_pos, float3 light_color, float light_intensity, float range)
{
    // Calculate light direction
    float3 light_dir = normalize(world_pos - light_pos);

    // View direction
    float3 view_dir = normalize(camera_pos - world_pos);

    // Halfway vector
    float3 half_way = normalize(-light_dir + view_dir);

    // Lambert diffuse
    float diff = max(dot(normal, -light_dir), 0.0f);
    
    // Calculate distance to light
    float distance = length(world_pos - light_pos);

    // Calculate attenuation based on distance and range
    float attenuation = saturate(1.0f - (distance / range));

    // Lambert reflectance
    float3 diffuse = albedo * light_color * diff * light_intensity * attenuation * ao;

    // Phong specular
    float n_dot_h = max(dot(normal, half_way), 0.0f);
    float shininess = lerp(256.0f, 2.0f, roughness);
    float spec_factor = pow(n_dot_h, shininess);

    // Specular reflectance
    float3 spec_reflectance 
        = specular * spec_factor * light_color * light_intensity * attenuation * ao * (1.0f - roughness);

    return diffuse + spec_reflectance; // Return combined color
}