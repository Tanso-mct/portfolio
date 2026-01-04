struct PS_IN
{
	float4 pos : SV_POSITION;
    float3 nor : NORMAL; // 法線
	float4 color : COLOR;
	float2 uv : TEXTURE0;
};

// Textureをスロット0の0番目のテクスチャレジスタに設定
Texture2D txDiffuse : register(t0, space0);

// Samplerをスロット0の0番目のサンプラレジスタに設定
SamplerState samLinear : register(s0, space0);

cbuffer ColorConfigBuffer : register(b4)
{
    int isColorEnabled;
    int padding[3];
};

float4 main(PS_IN input) : SV_Target
{
    if (isColorEnabled == 1)
    {
        return input.color;
    }

    float4 output = txDiffuse.Sample(samLinear, input.uv);
    return output;
}
