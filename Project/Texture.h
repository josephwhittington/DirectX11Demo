#pragma once

#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()
#define NULLIFYPTR(pointer) (pointer) = nullptr
#define RELEASEPTR(pointer) if ((pointer)) delete pointer

#include <string>
#include <d3d11.h>
#include "DDSTextureLoader.h"
#include "GraphicsAPIWrapper.h"

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	ID3D11ShaderResourceView** GetResourceViewAddress();
	ID3D11ShaderResourceView* GetResourceView();
	ID3D11SamplerState** GetSamplerStateAddress();
	ID3D11SamplerState* GetSamplerState();

	bool Initialize(GraphicsAPIWrapper* gwrapper, const char* filepath, bool isCubemap);

private:
	ID3D11ShaderResourceView* m_ResourceView;
	ID3D11SamplerState* m_SamplerState;

	bool isCubemap;
};