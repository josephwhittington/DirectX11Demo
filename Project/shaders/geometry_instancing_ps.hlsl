Texture2D diffuse : register(t0);
SamplerState samState : register(s0);

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEX;
};

float4 main(GSOutput input) : SV_TARGET
{
    float4 color = diffuse.Sample(samState, input.tex);
    
    return color;
}