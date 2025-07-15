Texture2D txDiffuse : register(t0);
SamplerState smpLinear : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return txDiffuse.Sample(smpLinear, input.texcoord);
}