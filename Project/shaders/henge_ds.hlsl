// Use row major matrices
#pragma pack_matrix(row_major)

// Input/Output control point
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

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
OutputVertex main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<OutputVertex, NUM_CONTROL_POINTS> patch)
{
    OutputVertex Output;

    Output.position = float4(patch[0].position.xyz * domain.x + patch[1].position.xyz * domain.y + patch[2].position.xyz * domain.z, 1);
    Output.normal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;
    Output.tex = float2(patch[0].tex * domain.x + patch[1].tex * domain.y + patch[2].tex * domain.z);
    
    Output.normal = mul(float4(Output.normal, 0), patch[0].worldMatrix);
    Output.position = mul(Output.position, patch[0].worldMatrix);
    Output.worldPosition = Output.position.xyz;
    Output.position = mul(Output.position, patch[0].viewMatrix);
    Output.position = mul(Output.position, patch[0].projectionMatrix);
    Output.cameraposition = patch[0].cameraposition;

    return Output;
}
