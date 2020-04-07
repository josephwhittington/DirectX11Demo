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
    float4x4 worldMatrix : WORLD;
    float4x4 viewMatrix : VIEW;
    float4x4 projectionMatrix : PROJ;
};

struct Out
{
    float4 position : SV_POSITION;
    float4x4 worldMatrix : WORLD;
    float4x4 viewMatrix : VIEW;
    float4x4 projectionMatrix : PROJ;
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
    // Original
    OutputVertex output;
    output.position = float4(input.position, 1);
    output.worldPosition = output.position; // Temp to avoid errors
    output.normal = input.normal; // Temp to avoid errors
    output.tex = input.texcoord;
    
    //output.position = mul(output.position, worldMatrix);
    //output.worldPosition = output.position.xyz;
    //output.position = mul(output.position, viewMatrix);
    //output.position = mul(output.position, projectionMatrix);
    //output.normal = mul(float4(input.normal, 0), worldMatrix);
    
    output.cameraposition = cameraPosition;
    output.worldMatrix = worldMatrix;
    output.viewMatrix = viewMatrix;
    output.projectionMatrix = projectionMatrix;
    // Original
    
    //// New
    //Out output;
    //output.position = float4(input.position.xyz, 1);
    //output.worldMatrix = worldMatrix;
    //output.viewMatrix = viewMatrix;
    //output.projectionMatrix = projectionMatrix;
    //// New
    
    return output;
}