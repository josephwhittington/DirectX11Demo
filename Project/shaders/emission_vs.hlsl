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
    float4 position : SV_POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
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
    
    output.worldPosition = mul(float4(input.position, 1), worldMatrix).xyz;
    output.position = mul(float4(output.worldPosition, 1), viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.normal = input.normal;
    output.cameraposition = cameraPosition;
    output.tex = input.texcoord;
    
    return output;
}