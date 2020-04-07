
TextureCube diffuse : register(t0);
SamplerState diffuseSam : register(s0);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldpos : WORLD;
};

float4 main(OutputVertex input) : SV_TARGET
{
    float4 color = diffuse.Sample(diffuseSam, input.worldpos);
    return color;
}