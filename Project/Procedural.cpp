#include "Procedural.h"

Mesh* Procedural::GenerateQuad(float length, const char* texturename)
{
	// Create the mesh
	Mesh* mesh = new Mesh;

	// Create 4 points
	std::vector<Vertex> vertices;
	std::vector<int> indices;


	// Pos(vec3), norm(vec3), uv(vec2)
	Vertex vert;
	XMFLOAT3 norm(0, 1, 0);
	vert.normal = norm;

	length /= 2;

	vert.position = XMFLOAT3(-length, 0, -length);
	vert.texcoords = XMFLOAT2(0, 1);
	vertices.push_back(vert);

	vert.position = XMFLOAT3(-length, 0, length);
	vert.texcoords = XMFLOAT2(0, 0);
	vertices.push_back(vert);

	vert.position = XMFLOAT3(length, 0, length);
	vert.texcoords = XMFLOAT2(1, 0);
	vertices.push_back(vert);
	
	vert.position = XMFLOAT3(length, 0, -length);
	vert.texcoords = XMFLOAT2(1, 1);
	vertices.push_back(vert);

	// Create the indices
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// Set the vertices
	mesh->SetVertices((Vertex*)vertices.data(), vertices.size());
	// Set the indices
	mesh->SetIndices((int*)indices.data(), indices.size());

	// Set the texture diffuse name
	std::string tex(texturename);
	mesh->SetTextureDiffuseName(tex.c_str());

	return mesh;
}

Mesh* Procedural::GenerateFullscreenQuad()
{
	// Create the mesh
	Mesh* mesh = new Mesh;

	// Create 4 points
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Vertex vert;
	XMFLOAT3 norm(0, 1, 0);
	vert.normal = norm;

	vert.position = XMFLOAT3(-1, 1, 0);
	vert.texcoords = XMFLOAT2(0, 0);
	vertices.push_back(vert);

	vert.position = XMFLOAT3(1, 1, 0);
	vert.texcoords = XMFLOAT2(1, 0);
	vertices.push_back(vert);

	vert.position = XMFLOAT3(1, -1, 0);
	vert.texcoords = XMFLOAT2(1, 1);
	vertices.push_back(vert);

	vert.position = XMFLOAT3(-1, -1, 0);
	vert.texcoords = XMFLOAT2(0, 1);
	vertices.push_back(vert);

	// Create the indices
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// Set the vertices
	mesh->SetVertices((Vertex*)vertices.data(), vertices.size());
	// Set the indices
	mesh->SetIndices((int*)indices.data(), indices.size());

	mesh->SetTextureDiffuseName("lari.dds");

	return mesh;
}

Mesh* Procedural::GenerateComplexQuad(float length, const char* texturename)
{
	Mesh* mesh = new Mesh;

	std::vector<Vertex> vertices;
	std::vector<int> indices;

	int xSize = 20, zSize = 20;
	float xadjust = xSize / 2, zadjust = zSize / 2;
	int vertCount = (xSize + 1) * (zSize + 1);

	vertices.resize(vertCount);
	indices.resize(xSize * zSize * 6);

	int i = 0;
	XMFLOAT3 normal(0, 1, 0); XMFLOAT2 tex(0, 0);
	for (int z = 0, i = 0; z <= zSize; z++)
	{
		for (int x = 0; x <= xSize; x++, i++)
		{
			vertices[i].position = XMFLOAT3(x - xadjust, 0, z - zadjust);
			vertices[i].normal = normal;
			vertices[i].texcoords = tex;
		}
	}

	int vert = 0, indexcount = 0;
	for (int z = 0; z < zSize; z++)
	{
		for (int x = 0; x < xSize; x++)
		{
			indices[indexcount + 0] = vert + 0;
			indices[indexcount + 1] = vert + xSize + 1;
			indices[indexcount + 2] = vert + 1;
			indices[indexcount + 3] = vert + 1;
			indices[indexcount + 4] = vert + xSize + 1;
			indices[indexcount + 5] = vert + xSize + 2;

			vert++; indexcount += 6;
		}
		vert++;
	}

	// Set the texture diffuse name
	std::string texname(texturename);
	mesh->SetTextureDiffuseName(texname.c_str());

	// Add shit to the mesh
	mesh->SetVertices(vertices.data(), vertices.size());
	mesh->SetIndices(indices.data(), indices.size());

	return mesh;
}