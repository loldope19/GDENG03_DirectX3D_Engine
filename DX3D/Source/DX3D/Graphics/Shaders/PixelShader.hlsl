struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer MaterialConstantBuffer : register(b0)
{
    float4 override_color;
    bool use_override_color;
    float3 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    if (use_override_color)
    {
        return override_color;
    }
    else
    {
        return input.color;
    }
}