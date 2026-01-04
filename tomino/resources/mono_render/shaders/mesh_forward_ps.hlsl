struct PS_IN
{
	float4 pos : SV_POSITION;
    float3 nor : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXTURE0;
};

cbuffer ColorConfig : register(b4)
{
    float4 colorConfigConfigColor;

    float colorConfigConfigColorWeight;
    float colorConfigVertexColorWeight;
    float colorConfigTextureColorWeight;
    float padding;
}

Texture2D txAlbedo : register(t0, space0);
SamplerState samLinear : register(s0, space0);

float4 main(PS_IN input) : SV_Target
{
    float4 configColor = colorConfigConfigColor * colorConfigConfigColorWeight;
    float4 vertexColor = input.color * colorConfigVertexColorWeight;
    float4 textureColor = txAlbedo.Sample(samLinear, input.uv) * colorConfigTextureColorWeight;

    return configColor + vertexColor + textureColor;
}
