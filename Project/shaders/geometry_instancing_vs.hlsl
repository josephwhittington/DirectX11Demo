// Use row major matrices
#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct OutputVertex
{
    float4 position : POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
    float4x4 world : WORLD;
    float4x4 view : VIEW;
    float4x4 proj : PROJECTION;
};

cbuffer SHADER_VARIABLES : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
    float seconds;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output;
    output.position = float4(input.position, 1);
    output.tex = input.texcoord;
    output.normal = float4(input.normal, 0);
    output.cameraposition = cameraPosition;
    output.worldPosition = output.position;
    
    output.world = worldMatrix;
    output.view = viewMatrix;
    output.proj = projectionMatrix;
    
    return output;
}