#include "full_screen.hlsli"

// Pixel Shader Output Structure
struct PS_OUT
{
    float4 color : SV_Target0;
};

// Linear Sampler
SamplerState sampler_linear : register(s0, space0);

// Post-Process texture
Texture2D post_process_texture : register(t0, space0);

// UI texture
Texture2D ui_texture : register(t1, space0);

// Pixel Shader Main Function
PS_OUT main(VS_OUT input)
{
    PS_OUT output;

    // Sample the post-process texture using input UVs and assign to color
    float4 post_process_color = post_process_texture.Sample(sampler_linear, input.uv);

    // Sample the UI texture using input UVs and assign to color
    float4 ui_color = ui_texture.Sample(sampler_linear, input.uv);

    // If both colors are fully transparent, discard the pixel
    if (post_process_color.a == 0.0 && ui_color.a == 0.0)
        clip(-1);

    // Composite UI from above with Alpha blend
    output.color.rgb = ui_color.a * ui_color.rgb + (1.0 - ui_color.a) * post_process_color.rgb;
    output.color.a = ui_color.a + (1.0 - ui_color.a) * post_process_color.a;

    return output;
}