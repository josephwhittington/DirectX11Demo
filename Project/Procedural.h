#pragma once

#include "Mesh.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

namespace Procedural
{
	Mesh* GenerateQuad(float length, const char* texturename);
	Mesh* GenerateFullscreenQuad();
	Mesh* GenerateComplexQuad(float length, const char* texturename);
}