#pragma pack_matrix(row_major)

struct InputVertex
{
    float4 postion : POSITION;
    float4 color : COLOR;
};

cbuffer ShaderVariables : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct OutPutVertex
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

OutPutVertex main( InputVertex input )
{
    OutPutVertex output = (OutPutVertex)0;
    
    output.color = input.color;
    output.position = input.postion;

    
    output.position = mul(output.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
	return output;
}