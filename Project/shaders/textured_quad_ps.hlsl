
Texture2D diffuse : register(t0);
SamplerState samState : register(s0);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
};

float4 main(OutputVertex input) : SV_TARGET
{   
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Discard the pixel if the alpha is low AF
    if (color.a == 0)
        discard;
    return color;
}