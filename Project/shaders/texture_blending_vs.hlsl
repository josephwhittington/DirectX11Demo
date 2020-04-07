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
    output.position = float4(input.position, 1);
    
    output.position = mul(output.position, worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.texcoord;
    output.normal = input.normal;
    output.cameraposition = cameraPosition;
    
    return output;
}

// Notes
/*
add float4 tangent to input vertex
add two float3 to output vertex
tangent and bitangent
in vertex sahder calculate bitangent (cross normal and tangent) mult by tangent w factor
multiply tbn by world matrix
tbn need a w of 0 when being multiplied by world matrix

*/