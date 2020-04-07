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

#define MAX_VERTS 4

[maxvertexcount(MAX_VERTS)]
void main(
	point OutputVertex input[1],
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element[4];
    
    input[0].position = mul(input[0].position, input[0].world);
    input[0].position = mul(input[0].position, input[0].view);
    
    
    // 1
    element[0].pos = input[0].position;
    element[0].pos.x -= 3;
    element[0].pos.y += 3;
    element[0].tex = float2(0, 0);
    
    // 2
    element[1].pos = input[0].position;
    element[1].pos.x += 3;
    element[1].pos.y += 3;
    element[1].tex = float2(1, 0);
    
    // 3
    element[2].pos = input[0].position;
    element[2].pos.x -= 3;
    element[2].pos.y -= 3;
    element[2].tex = float2(0, 1);
    
    // 4
    element[3].pos = input[0].position;
    element[3].pos.x += 3;
    element[3].pos.y -= 3;
    element[3].tex = float2(1, 1);
    
    for (int i = 0; i < 4; i++)
    {
        
        element[i].pos = mul(element[i].pos, input[0].proj);
        
        output.Append(element[i]);
    }
}