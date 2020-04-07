#include "Mesh.h"

Mesh::Mesh()
{
	NULLIFYPTR(m_VertexBuffer);
	NULLIFYPTR(m_VertexBufferLayout);
	NULLIFYPTR(m_InstancedLayout);
	NULLIFYPTR(m_Shader);
	NULLIFYPTR(m_TextureDiffuse);
	NULLIFYPTR(m_TextureNormal);
	NULLIFYPTR(m_ConstantBuffer);
	NULLIFYPTR(m_InstanceBuffer);
	NULLIFYPTR(m_IndexBuffer);

	// Set the first character of the  texture name to backtick to see if it's empty
	m_textureDiffuseName[0] = '`';
	m_textureNormalName[0] = '`';

	m_drawInstanced = false;

	// Fancy drawing shit
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Mesh::Mesh(const Mesh&)
{ 

}

Mesh::~Mesh()
{
	D3DRELEASE(m_VertexBuffer);
	D3DRELEASE(m_VertexBufferLayout);
	D3DRELEASE(m_InstancedLayout);
	D3DRELEASE(m_ConstantBuffer);
	D3DRELEASE(m_InstanceBuffer);
	D3DRELEASE(m_IndexBuffer);

	// Don't delete  the shader object, that's a shared pointer
	// Don't delete  the texture object, that's a shared pointer
}

bool Mesh::LoadData(ID3D11Device* device)
{
	if (m_vertices.size() == 0) return false;

	// Load Vertex data on the graphics card
	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subdata;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&subdata, sizeof(D3D11_SUBRESOURCE_DATA));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(Vertex) * GetNumVertices();
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subdata.pSysMem = m_vertices.data();

	device->CreateBuffer(&bDesc, &subdata, &m_VertexBuffer);

	// Load the shaders
	if (!LoadShaders(device))
		return false;

	HRESULT result;

	if (m_drawInstanced)
	{
		// Input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},

			// Instance stuff
			{ "INSTANCE_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		result = device->CreateInputLayout(
			inputElementDesc,
			ARRAYSIZE(inputElementDesc),
			m_Shader->GetShaderData().VertexShaderBuffer->GetBufferPointer(),
			m_Shader->GetShaderData().VertexShaderBuffer->GetBufferSize(),
			&m_VertexBufferLayout);

		if (FAILED(result)) return false;
	}
	else
	{
		// Input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		result = device->CreateInputLayout(
			inputElementDesc,
			ARRAYSIZE(inputElementDesc),
			m_Shader->GetShaderData().VertexShaderBuffer->GetBufferPointer(),
			m_Shader->GetShaderData().VertexShaderBuffer->GetBufferSize(),
			&m_VertexBufferLayout);

		if (FAILED(result)) return false;
	}
	
	// Mesh Index buffer
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = m_indices.size() * sizeof(int);

	subdata.pSysMem = m_indices.data();
	result = device->CreateBuffer(&bDesc, &subdata, &m_IndexBuffer);

	// Create constant buffer
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));

	// Byte width
	int bytewidth = sizeof(WORLD_VIEW_PROJECTION);
	// If the addition is not needed this evaluates to 0
	bytewidth += (bytewidth % 16 != 0) * (16 - (bytewidth % 16));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = bytewidth;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	result = device->CreateBuffer(&bDesc, nullptr, &m_ConstantBuffer);
	if (FAILED(result)) return false;

	if (m_drawInstanced)
	{
		D3D11_BUFFER_DESC instanceBuffer;
		D3D11_SUBRESOURCE_DATA instanceSubresource;

		ZeroMemory(&instanceBuffer, sizeof(D3D11_BUFFER_DESC));
		instanceBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBuffer.ByteWidth = sizeof(XMFLOAT3) * instancedPositions.size();
		instanceBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBuffer.CPUAccessFlags = 0;
		instanceBuffer.MiscFlags = 0;
		instanceBuffer.StructureByteStride = 0;
		instanceBuffer.Usage = D3D11_USAGE_DEFAULT;

		ZeroMemory(&instanceSubresource, sizeof(D3D11_SUBRESOURCE_DATA));
		instanceSubresource.pSysMem = instancedPositions.data();

		result = device->CreateBuffer(&instanceBuffer, &instanceSubresource, &m_InstanceBuffer);
		if (FAILED(result)) return false;
	}

	return true;
}

void Mesh::Frame(ID3D11DeviceContext* deviceContext)
{
	// IA config
	deviceContext->IASetInputLayout(m_VertexBufferLayout);

	if (m_drawInstanced)
	{
		UINT strides[] = { sizeof(Vertex), sizeof(XMFLOAT3) };
		UINT offsets[] = { 0, 0 };

		ID3D11Buffer* tempVertexBuffer[] = { m_VertexBuffer, m_InstanceBuffer };
		deviceContext->IASetVertexBuffers(0, 2, tempVertexBuffer, strides, offsets);
	}
	else
	{
		UINT strides[] = { sizeof(Vertex) };
		UINT offsets[] = { 0 };

		ID3D11Buffer* tempVertexBuffer[] = { m_VertexBuffer };
		deviceContext->IASetVertexBuffers(0, 1, tempVertexBuffer, strides, offsets);
	}
	
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(m_topology);

	// Vertex shader stage
	GWRAPPERSHADER_DATA data = m_Shader->GetShaderData();
	deviceContext->VSSetShader(data.Vertexshader, 0, 0);
	deviceContext->PSSetShader(data.Pixelshader, 0, 0);
	deviceContext->GSSetShader(data.GeometryShader, 0, 0);
	deviceContext->HSSetShader(data.HullShader, 0, 0);
	deviceContext->DSSetShader(data.DomainShader, 0, 0);
	

	// Send the matrix to constant buffer
	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	HRESULT result = deviceContext->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	memcpy(gpuBuffer.pData, &m_WVP, sizeof(m_WVP));
	deviceContext->Unmap(m_ConstantBuffer, 0);
	
	// Connect constant buffer to the pipeline
	ID3D11Buffer* constantbuffers[] = { m_ConstantBuffer };
	deviceContext->VSSetConstantBuffers(0, 1, constantbuffers);

	// Actually draw
	if (m_drawInstanced)
	{
		deviceContext->DrawIndexedInstanced(GetNumIndices(), instancedPositions.size(), 0, 0, 0);
	}
	else
	{
		deviceContext->DrawIndexed(GetNumIndices(), 0, 0);
	}
}

bool Mesh::LoadShaders(ID3D11Device* device)
{
	GWRAPPERSHADER_DATA data = m_Shader->GetShaderData();

	return true;
}

int Mesh::GetNumVertices()
{
	return m_vertices.size();
}

int Mesh::GetNumIndices()
{
	return m_indices.size();
}

const char* Mesh::GetTextureDiffuseName()
{
	return m_textureDiffuseName;
}

const char* Mesh::GetTextureNormalName()
{
	return m_textureNormalName;
}

std::vector<Vertex>* Mesh::GetVertexDataPointer()
{
	return &m_vertices;
}

bool Mesh::HasDiffuseTexture()
{
	return m_textureDiffuseName[0] == '`' ? false : true;
}

bool Mesh::HasNormalTexture()
{
	return m_textureNormalName[0] == '`' ? false : true;
}

void Mesh::SetVertices(Vertex* dataSrc, int numVertices)
{
	// Clear the current vertices
	m_vertices.clear();
	// Resize the vertices to the size needed to hold the incoming vertices
	m_vertices.resize(numVertices);
	// Compute bytesize
	int bytesize = numVertices * sizeof(Vertex);
	// Copy the data
	memcpy(m_vertices.data(), dataSrc, bytesize);
}

void Mesh::SetIndices(int* dataSrc, int numIndices)
{
	// Clear current indices
	m_indices.clear();
	// Resize the indices to the size needed to hold the indices
	m_indices.resize(numIndices);
	// Compute the bytesize
	int bytesize = numIndices * sizeof(int);
	// Copy the data
	memcpy(m_indices.data(), dataSrc, bytesize);
}

void Mesh::SetShader(Shader* shader)
{
	m_Shader = shader;
}

void Mesh::SetTextureDiffuseName(const char* texture)
{
	std::string s = texture;
	s = std::string("Assets/models/").append(s);

	strcpy_s(m_textureDiffuseName, s.c_str());
}

void Mesh::SetTextureNormalName(const char* texture)
{
	std::string s = texture;
	s = std::string("Assets/models/").append(s);

	strcpy_s(m_textureNormalName, s.c_str());
}

void Mesh::SetWorldMatrix(XMMATRIX* src)
{
	XMStoreFloat4x4(&m_WVP.world, *(src));
}

void Mesh::SetViewMatrix(XMMATRIX* src)
{
	XMStoreFloat4x4(&m_WVP.view, *(src));
}

void Mesh::SetProjectionMatrix(XMMATRIX* src)
{
	XMStoreFloat4x4(&m_WVP.projection, *(src));
}

void Mesh::SetCameraPosition(XMFLOAT3 cameraPosition)
{
	m_WVP.cameraPosition = cameraPosition;
}

void Mesh::SetTime(float time)
{
	m_WVP.seconds = time;
}

void Mesh::SetTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}

void Mesh::SetDrawInstanced(bool drawInstanced)
{
	m_drawInstanced = drawInstanced;
}

bool Mesh::GetDrawInstanced()
{
	return m_drawInstanced;
}

void Mesh::SetInstancedPositions(XMFLOAT3& startingposition, int numpositions)
{
	instancedPositions.resize(numpositions);

	memcpy(instancedPositions.data(), &startingposition, sizeof(XMFLOAT3) * numpositions);
}
