
Texture2D diffuse : register(t0);
SamplerState diffuseSam : register(s0);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float2 tex : TEXCOORD;
};

float4 main(OutputVertex input) : SV_TARGET
{
    //return input.color;
    return diffuse.Sample(diffuseSam, input.tex);
}