#include "Model.h"

Model::Model()
{
	NULLIFYPTR(m_Mesh);

	m_xrot = m_yrot = m_zrot = 0;

	m_xscale = m_yscale = m_zscale = 1;

	m_drawInstanced = false;
	m_isSkybox = false;

	// Set the number of textures to 2 / diffuse / normal
	m_Textures.resize(2);
	for (int i = 0; i < m_Textures.size(); i++)
	{
		m_Textures[i] = 0;
	}

	// RTT SHIT
	m_rttmodeenabled = false;
	m_useTexturedScene = false;
}

Model::Model(const Model&)
{

}

Model::~Model()
{
	RELEASEPTR(m_Mesh);

	// Loop through and delete all the shaders
	for (int i = 0; i < m_Shaders.size(); i++)
	{
		delete m_Shaders[i];
	}
	m_Shaders.clear();
	
	// Loop through and delete all the textures
	for (int i = 0; i < m_Textures.size(); i++)
	{
		if(m_Textures[i])
			delete m_Textures[i];
	}
	m_Textures.clear();
}

bool Model::Initialize(GraphicsAPIWrapper* gwrapper, Camera* camera, const char* meshname, const char* shadername, bool hasGeometryShader, bool hasTesselationShader)
{
	m_Mesh = ResourceManager::LoadWobjectMesh(meshname);

	// Set instancing
	m_Mesh->SetDrawInstanced(m_drawInstanced);

	m_Shaders.push_back(ResourceManager::LoadShader(gwrapper, shadername, false, hasGeometryShader, hasTesselationShader));

	// Initialize textures
	SetTextures(gwrapper);

	m_Mesh->SetCameraPosition(camera->GetPosition());
	m_Mesh->SetShader(m_Shaders[0]);
	m_Mesh->LoadData(gwrapper->GetDevice());
	return true;
}

bool Model::InitializeWithMesh(GraphicsAPIWrapper* gwrapper, Camera* camera, Mesh* mesh, const char* shadername, bool hasGeometryShader, bool hasTesselationShader)
{
	m_Mesh = mesh;

	// Set instancing
	m_drawInstanced = m_Mesh->GetDrawInstanced();

	m_Shaders.push_back(ResourceManager::LoadShader(gwrapper, shadername, false, hasGeometryShader, hasTesselationShader));

	// Initialize textures
	SetTextures(gwrapper);

	m_Mesh->SetCameraPosition(camera->GetPosition());
	m_Mesh->SetShader(m_Shaders[0]);
	m_Mesh->LoadData(gwrapper->GetDevice());

	return true;
}

void Model::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}

void Model::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 Model::GetPosition()
{
	return m_position;
}

void Model::RotateX(float deg)
{
	m_xrot = XMConvertToRadians(deg);
}

void Model::RotateY(float deg)
{
	m_yrot = XMConvertToRadians(deg);
}

void Model::RotateZ(float deg)
{
	m_zrot = XMConvertToRadians(deg);
}

void Model::ScaleXYZ(float x, float y, float z)
{
	m_xscale = x; m_yscale = y; m_zscale = z;
}

void Model::Frame(GraphicsAPIWrapper* gwrapper, FPSCamera* camera, double deltaTime, double milliselapsed)
{
	// Set the texture things
	std::vector<ID3D11ShaderResourceView*> shaderResourceViews;
	shaderResourceViews.resize(m_Textures.size());

	std::vector<ID3D11SamplerState*> samplerStates;
	samplerStates.resize(m_Textures.size());

	// Loop through textures and add shit
	// Don't set textures if in rtt mode

	if (!m_rttmodeenabled && !m_useTexturedScene)
	{
		for (int i = 0; i < m_Textures.size(); i++)
		{
			shaderResourceViews[i] = m_Textures[i]->GetResourceView();
			samplerStates[i] = m_Textures[i]->GetSamplerState();
		}

		gwrapper->GetDeviceContext()->PSSetShaderResources(0, m_Textures.size(), shaderResourceViews.data());
		gwrapper->GetDeviceContext()->PSSetSamplers(0, m_Textures.size(), samplerStates.data());
	}
	else
	{
		if (m_useTexturedScene)
			gwrapper->SetOfflineShaderSampler();
		else gwrapper->SetShaderSampler();
	}

	// Set the mesh's matrices
	// World view matrix projection

	XMMATRIX temp = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	temp = XMMatrixMultiply(XMMatrixScaling(m_xscale, m_yscale, m_zscale), temp);
	temp = XMMatrixMultiply(XMMatrixRotationX(m_xrot), temp);
	temp = XMMatrixMultiply(XMMatrixRotationY(m_yrot), temp);
	temp = XMMatrixMultiply(XMMatrixRotationZ(m_zrot), temp);
	m_Mesh->SetWorldMatrix(&temp);
	camera->GetViewMatrix(temp);
	m_Mesh->SetViewMatrix(&temp);
	temp = XMMatrixPerspectiveFovLH(camera->GetFOV(), gwrapper->GetAspectRatio(), camera->GetNear(), camera->GetFar());
	m_Mesh->SetProjectionMatrix(&temp);
	m_Mesh->SetCameraPosition(camera->GetPosition());
	m_Mesh->SetTime(float(milliselapsed / 1000));
	
	if (m_isTransparent)
	{
		gwrapper->EnableBlendState();
	}
	m_Mesh->Frame(gwrapper->GetDeviceContext());
	if (m_isTransparent)
	{
		gwrapper->DisableBlendState();
	}
}

Mesh* Model::GetMesh()
{
	return m_Mesh;
}

void Model::SetDrawInstanced(bool drawInstanced)
{
	m_drawInstanced = drawInstanced;
	m_Mesh->SetDrawInstanced(drawInstanced);
}

void Model::SetInstancedPositions(XMFLOAT3& startingposition, int numpositions)
{
	m_Mesh->SetInstancedPositions(startingposition, numpositions);
}

void Model::SetTransparency(bool mode)
{
	m_isTransparent = mode;
}

void Model::SetIsSkybox(bool mode)
{
	m_isSkybox = mode;
}

bool Model::GetIsSkybox()
{
	return m_isSkybox;
}

void Model::SetRTTEnabledMode(bool mode)
{
	m_rttmodeenabled = mode;
}

void Model::SetUseTexturedSceneMode(bool mode)
{
	m_useTexturedScene = mode;
}

void Model::SetTextures(GraphicsAPIWrapper* gwrapper)
{
	// Only do this shit if this thing isn't enabled
	if (!m_rttmodeenabled && !m_useTexturedScene)
	{
		// Initialize diffuse texture
		m_Textures[(int)TEXTURE_TYPE::DIFFUSE] = new Texture;
		if (m_Mesh->HasDiffuseTexture())
			m_Textures[(int)TEXTURE_TYPE::DIFFUSE]->Initialize(gwrapper, m_Mesh->GetTextureDiffuseName(), m_isSkybox);

		// Initialize normal texture
		m_Textures[(int)TEXTURE_TYPE::NORMAL_MAP] = new Texture;
		if (m_Mesh->HasNormalTexture())
			m_Textures[(int)TEXTURE_TYPE::NORMAL_MAP]->Initialize(gwrapper, m_Mesh->GetTextureNormalName(), m_isSkybox);
	}
}