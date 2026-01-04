
struct VS_IN
{
	float3 pos : POSITION0;
	float2 uv : TEXTURE0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXTURE0;
};

cbuffer ClipBuffer : register(b0) 
{
    float4x4 clipMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
	output.pos = float4(input.pos, 1.0f);

	output.pos = mul(output.pos, clipMatrix);
    output.uv = input.uv;

    return output;
}
