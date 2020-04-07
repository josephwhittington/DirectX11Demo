Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(OutputVertex input) : SV_TARGET
{
    float4 color = diffuse.Sample(diffuseSampler, input.texcoord);
    
    return color;
}