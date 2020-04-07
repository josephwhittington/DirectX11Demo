#pragma once

// Macros
#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()
// Macros

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

struct GWRAPPERSHADER_DATA
{
	ID3D11VertexShader* Vertexshader;
	ID3D11PixelShader* Pixelshader;
	ID3D11GeometryShader* GeometryShader;
	ID3D11HullShader* HullShader;
	ID3D11DomainShader* DomainShader;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;
	ID3D10Blob* GeometryShaderBuffer;
	ID3D10Blob* HullShaderBuffer;
	ID3D10Blob* DomainShaderBuffer;
};

struct SHADER_DESC
{
	ID3D11Device* device;
	char shaderName[128];
	bool hasGeometryShader;
	bool hasTesselationShader;
	bool useDefault;
};

class Shader
{
public:
	Shader();
	Shader(const Shader&);
	~Shader();

	bool Initialize(SHADER_DESC shaderDesc);

	GWRAPPERSHADER_DATA GetShaderData();

private:
	ID3D11VertexShader* m_Vertexshader;
	ID3D11PixelShader* m_Pixelshader;
	ID3D11GeometryShader* m_GeometryShader;
	ID3D11HullShader* m_Hullshader;
	ID3D11DomainShader* m_Domainshader;
	ID3D10Blob* m_VertexShaderBuffer;
	ID3D10Blob* m_PixelShaderBuffer;
	ID3D10Blob* m_GeometryShaderBuffer;
	ID3D10Blob* m_HullshaderBuffer;
	ID3D10Blob* m_DomainshaderBuffer;
};