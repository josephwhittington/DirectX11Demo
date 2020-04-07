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

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEX;
};

float4 main(GSOutput input) : SV_TARGET
{
    float4 color = diffuse.Sample(samState, input.tex);
    if(color.a < .2)
        discard;
    
    return color;
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}