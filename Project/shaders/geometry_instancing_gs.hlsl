// Use row major matrices
#pragma pack_matrix(row_major)

struct GSOutput
{
	float4 pos : SV_POSITION;
    float2 tex : TEX;
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

[maxvertexcount(6)]
void main(
	triangle OutputVertex input[3],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element[3];
    
    for (int i = 0; i < 3; i++)
    {
        float4 position = input[i].position;
        
        position = mul(position, input[i].world);
        position = mul(position, input[i].view);
        position = mul(position, input[i].proj);
        
        element[i].pos = position;
        element[i].tex = input[i].tex;
        
        output.Append(element[i]);
    }
    
    output.RestartStrip();
    
    for (int i = 0; i < 3; i++)
    {
        float4 position = input[i].position;
        position.y += 2;
        
        position = mul(position, input[i].world);
        position = mul(position, input[i].view);
        position = mul(position, input[i].proj);
        
        element[i].pos = position;
        element[i].tex = input[i].tex;
        
        output.Append(element[i]);
    }
}