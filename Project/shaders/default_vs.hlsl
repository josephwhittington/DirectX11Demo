// Three things have to match
// 1. C++ Vertex Struct
// 2. Input Layout
// 3. HLSL Vertex Struct

// Use row major matrices
#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 position : POSITION;
    float3 texcoord : TEXCOORD;
    float3 normal : NORMAL;
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
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.position = float4(input.position, 1);
    output.color.xyz = input.normal;
    
    // Do math here
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.texcoord = input.texcoord.xy;
    
    return output;
}