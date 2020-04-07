// Use row major matrices
#pragma pack_matrix(row_major)

// Input control point
// Output control point
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

cbuffer TESS_DATA : register(b0)
{
    int tessFactor;
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<OutputVertex, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
	// Insert code to compute Output here
    Output.EdgeTessFactor[0] =
	Output.EdgeTessFactor[1] =
	Output.EdgeTessFactor[2] =
	Output.InsideTessFactor = tessFactor; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
OutputVertex main(
	InputPatch<OutputVertex, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    //Og
    OutputVertex Output;

	// Insert code to compute Output here
    Output.position = ip[i].position;
    Output.worldPosition = ip[i].worldPosition;
    Output.cameraposition = ip[i].cameraposition;
    Output.normal = ip[i].normal;
    Output.tex = ip[i].tex;
    Output.worldMatrix = ip[0].worldMatrix;
    Output.viewMatrix = ip[0].viewMatrix;
    Output.projectionMatrix = ip[0].projectionMatrix;

    return Output;
}