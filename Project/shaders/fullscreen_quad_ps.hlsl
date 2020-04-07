
Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);

Texture2D depth : register(t1);
SamplerState depthSampler : register(s1);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
    float time : TIME;
};

struct Light
{
	float4 position, lightDirection;
	float4 ambient, diffuse, specular;
	unsigned int lightType;
	float ambientIntensity, diffuseIntensity, specularIntensity;
	float cosineInnerCone, cosineOuterCone;
	float lightRadius;
	int lightOn;
};

struct EFFECT_DATA
{
	int effectid;
};

cbuffer Lights : register(b0)
{
	Light light[16];
};

cbuffer Effects : register(b1)
{
	EFFECT_DATA effect;
}

float4 blur(OutputVertex input, float blurratio)
{
    //init color variable
    float4 col = 0;
    float invaspect = 768.0 / 1080.0;
    float bluraspect = 0.015;
    
    //float blurratio = 1;
    float blendratio = (blurratio) / 10;
    
    float4 ogcolor = diffuse.Sample(diffuseSampler, input.tex);
    
    col = ogcolor * (1 - blurratio);
    
    for (float index = 0; index < 10; index++)
    {
        float2 uv = input.tex + float2((index / 9 - 0.5) * bluraspect * invaspect, 0);
        
        // add color at position to color
        col += diffuse.Sample(diffuseSampler, uv) * blendratio;
    }
    //divide the sum of values by the amount of samples
    col = col;
    
    return col;
}

float4 ScreenWarp(OutputVertex input)
{
    float originalx = input.tex.x;
    //float time = input.time - floor(input.time) - 1;
    //time = 1 - time;
    float time = sin(input.time);
    
    input.tex.x += cos(input.tex.y * 10) * 0.11 * (0.5f - abs(input.tex.x - 0.5f)) * time;
    input.tex.y += cos(input.tex * 10) * 0.11 * (0.5f - abs(input.tex.y - 0.5f)) * time;
    
    float4 color = diffuse.Sample(diffuseSampler, input.tex);

    return color;
}

float4 Fog(OutputVertex input)
{
    float f = 10000.0;
    float n = 0.1;
    float z = (2 * n) / (f + n - depth.Sample(diffuseSampler, input.tex).x * (f - n));
    
    z = saturate(z + .08);

    float4 depth = float4(z, z, z, 1);
    
    // White fog
    float4 fogcolor = float4(1, 1, 1, 1);
    
    // Fuck it, use the depth factor as vfog factor
    float4 color = diffuse.Sample(diffuseSampler, input.tex);
    color = lerp(color, fogcolor, z);
    
    return color;
}

float4 depthoffield(OutputVertex input)
{
    float f = 10000.0;
    float n = 0.1;
    float z = (2 * n) / (f + n - depth.Sample(diffuseSampler, input.tex).x * (f - n));
    
    float targetdensity = .1;
    
    float bluramount = saturate(abs(z - targetdensity)/ .1);

    float4 color = blur(input, bluramount);

    return color;
}

float4 main(OutputVertex input) : SV_TARGET
{
	float4 color = diffuse.Sample(diffuseSampler, input.tex);
	
	switch (effect.effectid)
	{
		case 0:
			break;
		case 1:
			{
                return blur(input, 1);
				break;
			}
		case 2:
			{
				float3 lum = float3(0.299, 0.587, 0.114);
				float channel = dot(color.rgb, lum);
				color = float4(float3(channel, channel, channel), color.a);
				break;
			}
        case 3:
            {
                color = ScreenWarp(input);
                break;
            }
        case 4:
            {
                color = Fog(input);
                break;
            }
        case 5:
            {
                color = depthoffield(input);
                break;
            }
		default:
			break;
	}

	return color;
}