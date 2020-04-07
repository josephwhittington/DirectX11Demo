
Texture2D diffuse : register(t0);
SamplerState samState : register(s0);

TextureCube emission : register(t1);
SamplerState esampler : register(s1);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
    float3 reflectiondir : REFLECT;
};

float4 main(OutputVertex input) : SV_TARGET
{
    float4 color = float4(0, 0, 0, 0);
    color = emission.Sample(samState, input.reflectiondir);

    return color;
}
/* Note:
 * Normalizing the normal can be removed if the normal is normalized in the vertex shader, first
 */