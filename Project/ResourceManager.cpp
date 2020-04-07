#include "ResourceManager.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

// TODO: Remove buffer usage and copy directly into the file
Mesh* ResourceManager::LoadWobjectMesh(const char* meshname)
{
	Mesh* mesh = new Mesh;
	MeshHeader header;

	std::string smeshname = meshname;

	assert(smeshname.find(".fbx") == std::string::npos);

	std::ifstream file;
	file.open(meshname, std::ios::binary | std::ios::in);
	assert(file.is_open());

	// Read in the header
	file.read((char*)&header, sizeof(MeshHeader));
	std::string s = header.texturename;
	mesh->SetTextureDiffuseName(s.c_str());

	// Create a buffer to hold the data
	char* buffer = new char[header.vertexcount * (size_t)sizeof(Vertex)];

	// Read in the indices
	file.read((char*)buffer, header.indexcount * (size_t)sizeof(int));
	mesh->SetIndices((int*)buffer, header.indexcount);

	// Read in the vertices
	file.read((char*)buffer, header.vertexcount * (size_t)sizeof(Vertex));
	mesh->SetVertices((Vertex*)buffer, header.vertexcount);

	// Free memory
	delete[] buffer;

	file.close();
	return mesh;
}

Mesh* ResourceManager::LoadWobjectMeshInstanced(const char* meshname, XMFLOAT3& position, int numinstances)
{
	Mesh* mesh = new Mesh;
	MeshHeader header;

	std::string smeshname = meshname;

	assert(smeshname.find(".fbx") == std::string::npos);

	std::ifstream file;
	file.open(meshname, std::ios::binary | std::ios::in);
	assert(file.is_open());

	// Read in the header
	file.read((char*)&header, sizeof(MeshHeader));
	std::string s = header.texturename;
	mesh->SetTextureDiffuseName(s.c_str());

	// Create a buffer to hold the data
	char* buffer = new char[header.vertexcount * (size_t)sizeof(Vertex)];

	// Read in the indices
	file.read((char*)buffer, header.indexcount * (size_t)sizeof(int));
	mesh->SetIndices((int*)buffer, header.indexcount);

	// Read in the vertices
	file.read((char*)buffer, header.vertexcount * (size_t)sizeof(Vertex));
	mesh->SetVertices((Vertex*)buffer, header.vertexcount);

	// Free memory
	delete[] buffer;

	file.close();

	// Instancing stuff
	mesh->SetDrawInstanced(true);
	mesh->SetInstancedPositions(position, numinstances);

	return mesh;
}

Mesh* ResourceManager::LoadWobjectMeshWithNormalMap(const char* meshname, const char* normalmapname)
{
	Mesh* mesh = ResourceManager::LoadWobjectMesh(meshname);
	mesh->SetTextureNormalName(normalmapname);

	return mesh;
}

Mesh* ResourceManager::LoadStoneHenge()
{
	Mesh* mesh = new Mesh();

	std::vector<Vertex> verts;
	std::vector<int> indices;

	Vertex temp;
	for (int i = 0; i < 1457; i++)
	{
		temp.position = XMFLOAT3(StoneHenge_data[i].pos[0], StoneHenge_data[i].pos[1], StoneHenge_data[i].pos[2]);
		temp.texcoords = XMFLOAT2(StoneHenge_data[i].uvw[0], StoneHenge_data[i].uvw[1]);
		temp.normal = XMFLOAT3(StoneHenge_data[i].nrm[0], StoneHenge_data[i].nrm[1], StoneHenge_data[i].nrm[2]);

		verts.push_back(temp);
	}

	for (int i = 0; i < 2532; i++)
	{
		indices.push_back(StoneHenge_indicies[i]);
	}

	mesh->SetTextureDiffuseName("StoneHenge.dds");

	mesh->SetVertices(verts.data(), verts.size());
	mesh->SetIndices(indices.data(), indices.size());

	verts.clear();
	indices.clear();

	return mesh;
}

Shader* ResourceManager::LoadShader(GraphicsAPIWrapper* gwrapper, const char* shadername, bool use_default, bool hasGeometryShader, bool hasTesselationShader)
{
	// Create shader object
	Shader* shader = new Shader;

	SHADER_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.device = gwrapper->GetDevice();
	sdesc.useDefault = use_default;
	strcpy_s(sdesc.shaderName, shadername);
	sdesc.hasGeometryShader = hasGeometryShader;
	sdesc.hasTesselationShader = hasTesselationShader;

	shader->Initialize(sdesc);

	return shader;
}