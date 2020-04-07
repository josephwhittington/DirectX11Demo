#pragma once

// Macros
#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()
#define NULLIFYPTR(pointer) (pointer) = nullptr
// Macros

#include <vector>

#include "Shader.h"
#include "Texture.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoords;
};

struct WORLD_VIEW_PROJECTION
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT3 cameraPosition;
	float seconds;
};

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh&);
	~Mesh();

	bool LoadData(ID3D11Device* device);
	void Frame(ID3D11DeviceContext* deviceContext);

	int GetNumVertices();
	int GetNumIndices();
	const char* GetTextureDiffuseName();
	const char* GetTextureNormalName();
	std::vector<Vertex>* GetVertexDataPointer();

	bool HasDiffuseTexture();
	bool HasNormalTexture();

	void SetVertices(Vertex* dataSrc, int numVertices);
	void SetIndices(int* dataSrc, int numIndices);
	void SetShader(Shader* shader);
	void SetTextureDiffuseName(const char*);
	void SetTextureNormalName(const char*);
	void SetWorldMatrix(XMMATRIX* src);
	void SetViewMatrix(XMMATRIX* src);
	void SetProjectionMatrix(XMMATRIX* src);
	void SetCameraPosition(XMFLOAT3 cameraPosition);
	void SetTime(float time);
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY topology);

	void SetDrawInstanced(bool drawInstanced);
	bool GetDrawInstanced();
	void SetInstancedPositions(XMFLOAT3& startingposition, int numpositions);
private:
	bool LoadShaders(ID3D11Device* device);


public:
	char m_textureDiffuseName[256];
	char m_textureNormalName[256];

	WORLD_VIEW_PROJECTION m_WVP;
	std::vector<Vertex> m_vertices;
	std::vector<int> m_indices;
	Shader* m_Shader;
	Texture* m_TextureDiffuse;
	Texture* m_TextureNormal;

	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	ID3D11InputLayout* m_VertexBufferLayout;
	ID3D11InputLayout* m_InstancedLayout;
	// Shader variables
	ID3D11Buffer* m_ConstantBuffer;
	ID3D11Buffer* m_InstanceBuffer;

	// Instancing
	bool m_drawInstanced;
	std::vector<XMFLOAT3> instancedPositions;

	// Fancy drawing shit
	D3D_PRIMITIVE_TOPOLOGY m_topology;
};

// TODO: Add a single function to set the 3 matrices using enums

/* Usage - adding this because I keep fucking it up and I'm tired & it's like 4 am
 * 1. Load index data
 * 2. Load vertex data
 * 3. Initialize and load shaders
 * 4. Set World, View, Proj matrices
 * 5. Set shader pointer in mesh object
 * 6. Load the mesh into the graphics card
 */