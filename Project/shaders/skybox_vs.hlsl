// Three things have to match
// 1. C++ Vertex Struct
// 2. Input Layout
// 3. HLSL Vertex Struct

// Use row major matrices
#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

cbuffer SHADER_VARIABLES : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldpos : WORLD;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.position = float4(input.position, 1);
    
    // Do math here
    output.worldpos = input.position;
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    return output;
}