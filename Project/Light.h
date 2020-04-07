#pragma once

#include <DirectXMath.h>
using namespace DirectX;

enum class LIGHTTYPE
{
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2
};

struct Light
{
	XMFLOAT4 position, lightDirection;
	XMFLOAT4 ambient, diffuse, specular;
	unsigned int lightType;
	float ambientIntensity, diffuseIntensity, specularIntensity;
	float cosineInnerCone, cosineOuterCone;
	float lightRadius;
	int lightOn;
};