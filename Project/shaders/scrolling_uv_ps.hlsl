Texture2D diffuse : register(t0);
Texture2D normalMap : register(t1);
SamplerState diffuseSampler : register(s0);
SamplerState normalSampler : register(s1);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
    float seconds : TIME;
};

struct Light
{
    float4 position, lightDirection;
    float4 ambient, diffuse, specular;
    unsigned int lightType;
    float ambientIntensity, diffuseIntensity, specularIntensity;
    float cosineInnerCone, cosineOuterCone;
    float lightRadius;
    int lightOn;
};

cbuffer Lights
{
    Light light[16];
};

float4 main(OutputVertex input) : SV_TARGET
{
    input.tex.x += sin(input.tex.y + input.seconds + input.position.z);
    float4 color = diffuse.Sample(diffuseSampler, input.tex);
    
    return color;
}

/*
sample normal from normal map 
stretch out normal that was sampled
multiply mapped normal by tbn matrix
use this new normal for lighting
normalize all tbn vectors in pixel shader
*/