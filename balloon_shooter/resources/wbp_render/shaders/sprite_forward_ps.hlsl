struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXTURE0;
};

Texture2D txDiffuse : register(t0, space0);
SamplerState samLinear : register(s0, space0);

cbuffer ColorConfigBuffer : register(b1)
{
    float4 color;
    int isColorEnabled;
    int padding[3];
};

float4 main(PS_IN input) : SV_Target
{
    if (isColorEnabled == 1)
    {
        return color;
    }

    float4 output = txDiffuse.Sample(samLinear, input.uv);
    return output;
}
