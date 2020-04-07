#include "Texture.h"

Texture::Texture()
{
	NULLIFYPTR(m_ResourceView);
	NULLIFYPTR(m_SamplerState);
}

Texture::Texture(const Texture&)
{

}

Texture::~Texture()
{
	D3DRELEASE(m_ResourceView);
	D3DRELEASE(m_SamplerState);
}

ID3D11ShaderResourceView** Texture::GetResourceViewAddress()
{
	return &m_ResourceView;
}

ID3D11ShaderResourceView* Texture::GetResourceView()
{
	return m_ResourceView;
}

ID3D11SamplerState** Texture::GetSamplerStateAddress()
{
	return &m_SamplerState;
}

ID3D11SamplerState* Texture::GetSamplerState()
{
	return m_SamplerState;
}

bool Texture::Initialize(GraphicsAPIWrapper* gwrapper, const char* filepath, bool isSkybox)
{
	// Construct wide string with filename
	std::string spath = filepath;
	std::wstring wpath = std::wstring(spath.begin(), spath.end());

	// Load the file from memory
	isCubemap = isSkybox;

	HRESULT result;

	result = DirectX::CreateDDSTextureFromFile(gwrapper->GetDevice(), wpath.c_str(), nullptr, &m_ResourceView);

	// Create texture sample state
	D3D11_SAMPLER_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SAMPLER_DESC));
	sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sdesc.MinLOD = 0;
	sdesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the sampler state
	result = gwrapper->GetDevice()->CreateSamplerState(&sdesc, &m_SamplerState);

	return false;
}
/*FINISH THE TEXTURE RIGHT HERE*/