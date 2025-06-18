struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

cbuffer LightConstants : register(b1)
{
    float2 lightScreenPos;      // mouse position in client area pixel coordinates
    float2 screenResolution;    // client area width and height
    // float lightIntensity;    // OPTIONAL: if u want to control brightness
};

float4 main(PS_INPUT input) : SV_Target
{
    float linearAttenuationFactor = 0.05;
    float quadraticFactor = 0.01;
    float lightIntensity = 2.5f;
    
    float2 pixelScreenCoord = input.Position.xy;
    float dist = distance(pixelScreenCoord, lightScreenPos);        // check distance b/w pixel and cursor
    
    float attenuation = 1.0 / (1.0 + linearAttenuationFactor * dist + quadraticFactor * dist * dist);
    attenuation = saturate(attenuation * lightIntensity);                     // saturate means clamp apparently

    float4 outputColor = input.Color;
    // outputColor.rgb *= attenuation; // modulate object color by light

    return outputColor;
}