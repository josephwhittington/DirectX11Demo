#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Mesh.h"
#include "assets/StoneHenge.h"

// Mesh header struct
struct MeshHeader
{
	int indexcount, vertexcount;
	int indexstart, vertexstart;
	char texturename[256];
};

static class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	static Mesh* LoadWobjectMesh(const char* meshname);
	static Mesh* LoadWobjectMeshInstanced(const char* meshname, XMFLOAT3& position, int numinstances);
	static Mesh* LoadWobjectMeshWithNormalMap(const char* meshname, const char* normalmapname);
	static Mesh* LoadStoneHenge();
	static Shader* LoadShader(GraphicsAPIWrapper* gwrapper, const char* shadername, bool use_default, bool hasGeometryShader, bool hasTesselationShader);
};