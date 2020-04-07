#include "Shader.h"

Shader::Shader()
{
	m_Vertexshader = nullptr;
	m_Pixelshader = nullptr;
	m_GeometryShader = nullptr;
	m_Hullshader = nullptr;
	m_Domainshader = nullptr;
	m_VertexShaderBuffer = nullptr;
	m_PixelShaderBuffer = nullptr;
	m_GeometryShaderBuffer = nullptr;
	m_HullshaderBuffer = nullptr;
	m_DomainshaderBuffer = nullptr;
}

Shader::Shader(const Shader&)
{

}

Shader::~Shader()
{
	D3DRELEASE(m_Vertexshader);
	D3DRELEASE(m_Pixelshader);
	D3DRELEASE(m_GeometryShader);
	D3DRELEASE(m_Hullshader);
	D3DRELEASE(m_Domainshader);
	D3DRELEASE(m_VertexShaderBuffer);
	D3DRELEASE(m_PixelShaderBuffer);
	D3DRELEASE(m_GeometryShaderBuffer);
	D3DRELEASE(m_HullshaderBuffer);
	D3DRELEASE(m_DomainshaderBuffer);
}

bool Shader::Initialize(SHADER_DESC shaderDesc)
{
	// Read Shaders from file
   // Initialize the pointers to null
	ID3D10Blob* errorMessage = nullptr;

	// Construct shader names
	std::string vshader, pshader, gshader, hshader, dshader;
	vshader = std::string("shaders/").append(std::string(shaderDesc.shaderName).append("_vs.hlsl"));
	pshader = std::string("shaders/").append(std::string(shaderDesc.shaderName).append("_ps.hlsl"));
	gshader = std::string("shaders/").append(std::string(shaderDesc.shaderName).append("_gs.hlsl"));
	hshader = std::string("shaders/").append(std::string(shaderDesc.shaderName).append("_hs.hlsl"));
	dshader = std::string("shaders/").append(std::string(shaderDesc.shaderName).append("_ds.hlsl"));

	// Convert shader names to wstrings
	std::wstring vertexshader, pixelshader, geometryShader, hullShader, domainShader;
	vertexshader = std::wstring(vshader.begin(), vshader.end());
	pixelshader = std::wstring(pshader.begin(), pshader.end());
	geometryShader = std::wstring(gshader.begin(), gshader.end());
	hullShader = std::wstring(hshader.begin(), hshader.end());
	domainShader = std::wstring(dshader.begin(), dshader.end());

	LPCWSTR vertexShaderName, pixelShaderName, geometryShaderName, hullShaderName, domainShaderName;
	vertexShaderName = vertexshader.c_str();
	pixelShaderName = pixelshader.c_str();
	geometryShaderName = geometryShader.c_str();
	hullShaderName = hullShader.c_str();
	domainShaderName = domainShader.c_str();

	// Set default shaders
	if (shaderDesc.useDefault)
	{
		vertexShaderName = L"shaders/default_vs.hlsl";
		pixelShaderName = L"shaders/default_ps.hlsl";
	}

	HRESULT result = D3DCompileFromFile(vertexShaderName, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG, 0,
		&m_VertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			// Pop a message up on the screen to notify the user to check the text file for compile errors.
			//MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"", MB_OK);
		}
	}

	if (shaderDesc.hasGeometryShader)
	{
		result = D3DCompileFromFile(geometryShaderName, NULL, NULL, "main", "gs_5_0", D3DCOMPILE_DEBUG, 0, &m_GeometryShaderBuffer, &errorMessage);

		if (FAILED(result))
		{
			if (errorMessage)
			{
				// Pop a message up on the screen to notify the user to check the text file for compile errors.
				//MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"", MB_OK);
			}
		}
	}

	if (shaderDesc.hasTesselationShader)
	{
		result = D3DCompileFromFile(hullShaderName, NULL, NULL, "main", "hs_5_0", D3DCOMPILE_DEBUG, 0, &m_HullshaderBuffer, &errorMessage);

		if (FAILED(result))
		{
			if (errorMessage)
			{
				// Pop a message up on the screen to notify the user to check the text file for compile errors.
				//MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"", MB_OK);
			}
		}

		result = D3DCompileFromFile(domainShaderName, NULL, NULL, "main", "ds_5_0", D3DCOMPILE_DEBUG, 0, &m_DomainshaderBuffer, &errorMessage);

		if (FAILED(result))
		{
			if (errorMessage)
			{
				// Pop a message up on the screen to notify the user to check the text file for compile errors.
				//MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"", MB_OK);
			}
		}
	}

	result = D3DCompileFromFile(pixelShaderName, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG, 0,
		&m_PixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			// Pop a message up on the screen to notify the user to check the text file for compile errors.
			//MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"", MB_OK);
		}
	}

	// Create the vertex shader from the buffer.
	result = shaderDesc.device->CreateVertexShader(m_VertexShaderBuffer->GetBufferPointer(), m_VertexShaderBuffer->GetBufferSize(), NULL, &m_Vertexshader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = shaderDesc.device->CreatePixelShader(m_PixelShaderBuffer->GetBufferPointer(), m_PixelShaderBuffer->GetBufferSize(), NULL, &m_Pixelshader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the geometry shader from the buffer.
	if (shaderDesc.hasGeometryShader)
	{
		result = shaderDesc.device->CreateGeometryShader(m_GeometryShaderBuffer->GetBufferPointer(), m_GeometryShaderBuffer->GetBufferSize(), NULL, &m_GeometryShader);
		if (FAILED(result))
		{
			return false;
		}
	}

	// Create hull and domain shaders from buffer
	if (shaderDesc.hasTesselationShader)
	{
		result = shaderDesc.device->CreateHullShader(m_HullshaderBuffer->GetBufferPointer(), m_HullshaderBuffer->GetBufferSize(), NULL, &m_Hullshader);
		if (FAILED(result))
		{
			return false;
		}

		result = shaderDesc.device->CreateDomainShader(m_DomainshaderBuffer->GetBufferPointer(), m_DomainshaderBuffer->GetBufferSize(), NULL, &m_Domainshader);
		if (FAILED(result))
		{
			return false;
		}
	}
	// Read Shaders from file

	return true;
}

GWRAPPERSHADER_DATA Shader::GetShaderData()
{
	GWRAPPERSHADER_DATA data = { 0 };

	data.Pixelshader = m_Pixelshader;
	data.PixelShaderBuffer = m_PixelShaderBuffer;
	data.Vertexshader = m_Vertexshader;
	data.VertexShaderBuffer = m_VertexShaderBuffer;
	data.GeometryShader = m_GeometryShader;
	data.GeometryShaderBuffer = m_GeometryShaderBuffer;
	data.HullShader = m_Hullshader;
	data.HullShaderBuffer = m_HullshaderBuffer;
	data.DomainShader = m_Domainshader;
	data.DomainShaderBuffer = m_DomainshaderBuffer;

	return data;
}
