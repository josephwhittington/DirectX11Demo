#include "GraphicsAPIWrapper.h"

GraphicsAPIWrapper::GraphicsAPIWrapper()
{
	NULLIFYPTR(m_Device);
	NULLIFYPTR(m_DeviceContext);
	NULLIFYPTR(m_SwapChain);

	NULLIFYPTR(m_BackBuffer);
	NULLIFYPTR(m_RenderTargetView);

	NULLIFYPTR(m_ZBuffer);
	NULLIFYPTR(m_DepthStencil);

	NULLIFYPTR(m_RasterState);
	NULLIFYPTR(m_WireframeRasterState);
	NULLIFYPTR(m_BlendState);
	NULLIFYPTR(m_BlendStateDefault);

	// Set clear screen color
	m_clearScreenColor[0] = 0;
	m_clearScreenColor[1] = 0;
	m_clearScreenColor[2] = 0;
	m_clearScreenColor[3] = 1;

	// Depth buffer
	depthBufferEnabled = false;


	// Render to texture shit
	NULLIFYPTR(m_RenderTexture);
	NULLIFYPTR(m_RenderTextureZBuffer);
	NULLIFYPTR(m_TextureRTV);
	NULLIFYPTR(m_TextureDepthStencil);
	NULLIFYPTR(m_TextureShaderResourceView);
	NULLIFYPTR(m_Sampler);

	m_rtWidth = 2000;
	m_rtHeight = 2000;

	m_screenwidth = 1080;
	m_screenheight = 768;

	m_rttmodeEnabled = false;

	// Full screen RTT
	NULLIFYPTR(m_fullscreenTexture);
	NULLIFYPTR(m_fullscreenTextureZBuffer);
	NULLIFYPTR(m_fullscreenTextureRTV);
	NULLIFYPTR(m_fullscreenTextureDepthStencil);
	NULLIFYPTR(m_fullscreenTextureShaderResourceView);
	NULLIFYPTR(m_fullscreenTextureZBufferShaderResourceView);
	NULLIFYPTR(m_fullscreenTextureSampler);

	// View state
	NULLIFYPTR(m_DefaultState);
	NULLIFYPTR(m_TransparentState);

	m_rttDepthEnabled = true;

	m_wireframeModeEnabled = false;
}

GraphicsAPIWrapper::GraphicsAPIWrapper(const GraphicsAPIWrapper&)
{

}

GraphicsAPIWrapper::~GraphicsAPIWrapper()
{
}

bool GraphicsAPIWrapper::Initialize(HWND hWnd, int windowwidth, int windowheight, bool fullscreen, bool vsync)
{
	// Set member variables

	// Initialize API
	// Attach d3d to the window
	D3D_FEATURE_LEVEL DX11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap.BufferCount = 1;
	swap.OutputWindow = hWnd;
	swap.Windowed = true;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = windowwidth;
	swap.BufferDesc.Height = windowheight;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.SampleDesc.Count = 4;
	swap.SampleDesc.Quality = 0;
	swap.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	m_aspectRatio = swap.BufferDesc.Width / (float)swap.BufferDesc.Height;

	HRESULT result;

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &DX11, 1, D3D11_SDK_VERSION, &swap, &m_SwapChain, &m_Device, 0, &m_DeviceContext);

	ID3D11Resource* backbuffer;
	result = m_SwapChain->GetBuffer(0, __uuidof(backbuffer), (void**)&backbuffer);
	result = m_Device->CreateRenderTargetView(backbuffer, NULL, &m_RenderTargetView);

	// Release the resource to decrement the counter by one
	// This is necessary to keep the thing from leaking memory
	backbuffer->Release();

	// Setup viewport
	m_viewport.Width = (unsigned int)swap.BufferDesc.Width;
	m_viewport.Height = swap.BufferDesc.Height;
	m_viewport.TopLeftY = m_viewport.TopLeftX = 0;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	// Raster state
	ZeroMemory(&m_rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	m_rasterDesc.FrontCounterClockwise = false;
	m_rasterDesc.DepthBiasClamp = 1;
	m_rasterDesc.DepthBias = 0;
	m_rasterDesc.SlopeScaledDepthBias = 0;
	m_rasterDesc.DepthClipEnable = true;
	m_rasterDesc.FillMode = D3D11_FILL_SOLID;
	m_rasterDesc.CullMode = D3D11_CULL_BACK;
	m_rasterDesc.AntialiasedLineEnable = true;
	m_rasterDesc.MultisampleEnable = true;

	result = m_Device->CreateRasterizerState(&m_rasterDesc, &m_RasterState);

	// Wire frame Raster state
	D3D11_RASTERIZER_DESC m_rasterDesc;
	ZeroMemory(&m_rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	m_rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_rasterDesc.CullMode = D3D11_CULL_NONE;
	m_rasterDesc.DepthClipEnable = true;

	result = m_Device->CreateRasterizerState(&m_rasterDesc, &m_WireframeRasterState);

	m_DeviceContext->RSSetState(m_RasterState);
	m_DeviceContext->RSSetViewports(1, &m_viewport);

	// Blend state
	ZeroMemory(&m_blendDesc, sizeof(D3D11_BLEND_DESC));
	m_blendDesc.AlphaToCoverageEnable = true;
	m_blendDesc.IndependentBlendEnable = true;
	m_blendDesc.RenderTarget[0].BlendEnable = true;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	m_blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	// Src alpha/ Inverse src alpha - dark
	// Src alpha dest - additive
	m_blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	result = m_Device->CreateBlendState(&m_blendDesc, &m_BlendState);

	m_blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	m_blendDesc.AlphaToCoverageEnable = false;
	m_blendDesc.IndependentBlendEnable = false;
	m_blendDesc.RenderTarget[0].BlendEnable = false;
	result = m_Device->CreateBlendState(&m_blendDesc, &m_BlendStateDefault);

	// Z buffer 
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 4;
	zDesc.SampleDesc.Quality = 0;

	result = m_Device->CreateTexture2D(&zDesc, nullptr, &m_ZBuffer);

	result = m_Device->CreateDepthStencilView(m_ZBuffer, nullptr, &m_DepthStencil);

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

	result = m_Device->CreateSamplerState(&sdesc, &m_Sampler);

	// Create texture sample state
	ZeroMemory(&sdesc, sizeof(D3D11_SAMPLER_DESC));
	sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sdesc.MinLOD = 0;
	sdesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the sampler state
	result = m_Device->CreateSamplerState(&sdesc, &m_fullscreenTextureSampler);

	ConfigureRTT(1028, 1028);
	ConfigureFullScreenRTT(windowwidth, windowheight);

	return true;
}

void GraphicsAPIWrapper::BeginScene()
{
	if (m_wireframeModeEnabled)
		m_DeviceContext->RSSetState(m_WireframeRasterState);
	else m_DeviceContext->RSSetState(m_RasterState);

	if (m_rttmodeEnabled)
	{
		ID3D11RenderTargetView* tempRTV[] = { m_TextureRTV };

		if (depthBufferEnabled)
			m_DeviceContext->OMSetRenderTargets(1, tempRTV, m_TextureDepthStencil); // Depth stencil goes here
		else m_DeviceContext->OMSetRenderTargets(1, tempRTV, nullptr);

		// Clear the back buffer.
		m_DeviceContext->ClearRenderTargetView(m_TextureRTV, m_clearScreenColor);
		m_DeviceContext->ClearDepthStencilView(m_TextureDepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
	}
	else
	{
		m_DeviceContext->RSSetState(m_RasterState);

		ID3D11RenderTargetView* tempRTV[] = { m_RenderTargetView };

		if (depthBufferEnabled)
			m_DeviceContext->OMSetRenderTargets(1, tempRTV, m_DepthStencil); // Depth stencil goes here
		else m_DeviceContext->OMSetRenderTargets(1, tempRTV, nullptr);

		// Clear the back buffer.
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_clearScreenColor);
		m_DeviceContext->ClearDepthStencilView(m_DepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
	}
}

void GraphicsAPIWrapper::EndScene()
{
	m_SwapChain->Present(0, 0);

	if (m_wireframeModeEnabled)
		m_DeviceContext->RSSetState(m_WireframeRasterState);
	else m_DeviceContext->RSSetState(m_RasterState);
}

IDXGISwapChain* GraphicsAPIWrapper::GetSwapChain()
{
	return m_SwapChain;
}

ID3D11Device* GraphicsAPIWrapper::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* GraphicsAPIWrapper::GetDeviceContext()
{
	return m_DeviceContext;
}

float GraphicsAPIWrapper::GetAspectRatio()
{
	return m_aspectRatio;
}

void GraphicsAPIWrapper::SetAspectRatio(float aspect)
{
	m_aspectRatio = aspect;
}

void GraphicsAPIWrapper::SetFullScreen(bool mode)
{
	m_fullscreen = mode;

	/*if (mode)
	{
		m_SwapChain->SetFullscreenState(true, nullptr);
	}
	else
	{
		m_SwapChain->SetFullscreenState(false, nullptr);
	}*/
}

bool GraphicsAPIWrapper::GetFullScreen()
{
	return m_fullscreen;
}

void GraphicsAPIWrapper::ShutDown()
{
	D3DRELEASE(m_Device);
	D3DRELEASE(m_DeviceContext);
	D3DRELEASE(m_SwapChain);

	D3DRELEASE(m_RenderTargetView);
	D3DRELEASE(m_BackBuffer);

	D3DRELEASE(m_ZBuffer);
	D3DRELEASE(m_DepthStencil);

	D3DRELEASE(m_RasterState);
	D3DRELEASE(m_WireframeRasterState);
	D3DRELEASE(m_BlendState);
	D3DRELEASE(m_BlendStateDefault);

	// rENDER TO TEXTURE
	D3DRELEASE(m_RenderTexture);
	D3DRELEASE(m_RenderTextureZBuffer);
	D3DRELEASE(m_TextureRTV);
	D3DRELEASE(m_TextureDepthStencil);
	D3DRELEASE(m_TextureShaderResourceView);
	D3DRELEASE(m_Sampler);

	// FULL SCREEN RTT
	D3DRELEASE(m_fullscreenTexture);
	D3DRELEASE(m_fullscreenTextureZBuffer);
	D3DRELEASE(m_fullscreenTextureRTV);
	D3DRELEASE(m_fullscreenTextureDepthStencil);
	D3DRELEASE(m_fullscreenTextureShaderResourceView);
	D3DRELEASE(m_fullscreenTextureZBufferShaderResourceView);
	D3DRELEASE(m_fullscreenTextureSampler);

	// View state
	D3DRELEASE(m_DefaultState);
	D3DRELEASE(m_TransparentState);
}

void GraphicsAPIWrapper::SetWireframeMode(bool mode)
{
	m_wireframeModeEnabled = mode;
}

bool GraphicsAPIWrapper::GetWireframeMode()
{
	return m_wireframeModeEnabled;
}

void GraphicsAPIWrapper::ConfigureRTT(int width, int height)
{
	HRESULT result;

	m_rtWidth = width;
	m_rtHeight = height;

	// Render to texture shit
	D3D11_TEXTURE2D_DESC rttdesc;
	ZeroMemory(&rttdesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttdesc.Width = m_rtWidth;
	rttdesc.Height = m_rtHeight;
	rttdesc.MipLevels = 1;
	rttdesc.ArraySize = 1;
	rttdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rttdesc.SampleDesc.Count = 1;
	rttdesc.SampleDesc.Quality = 0;
	rttdesc.Usage = D3D11_USAGE_DEFAULT;
	rttdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttdesc.CPUAccessFlags = 0;
	rttdesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the texture
	m_Device->CreateTexture2D(&rttdesc, nullptr, &m_RenderTexture);

	// Create render target
	D3D11_RENDER_TARGET_VIEW_DESC RT_DESC;
	ZeroMemory(&RT_DESC, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RT_DESC.Format = rttdesc.Format;
	RT_DESC.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RT_DESC.Texture2D.MipSlice = 0;

	result = m_Device->CreateRenderTargetView(m_RenderTexture, &RT_DESC, &m_TextureRTV);

	// Create the z buffer texture
	D3D11_TEXTURE2D_DESC rttzdesc;
	ZeroMemory(&rttzdesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttzdesc.ArraySize = 1;
	rttzdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	rttzdesc.Width = rttdesc.Width;
	rttzdesc.Height = rttdesc.Height;
	rttzdesc.Usage = D3D11_USAGE_DEFAULT;
	rttzdesc.Format = DXGI_FORMAT_D32_FLOAT;
	rttzdesc.MipLevels = 1;
	rttzdesc.SampleDesc.Count = 1;
	rttzdesc.SampleDesc.Quality = 0;
	m_Device->CreateTexture2D(&rttzdesc, nullptr, &m_RenderTextureZBuffer);

	// Create second z buffer
	result = m_Device->CreateDepthStencilView(m_RenderTextureZBuffer, nullptr, &m_TextureDepthStencil);

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC texShaderDesc;
	ZeroMemory(&texShaderDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	texShaderDesc.Format = rttdesc.Format;
	texShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	texShaderDesc.Texture2D.MostDetailedMip = 0;
	texShaderDesc.Texture2D.MipLevels = 1;

	// Create shader resource view
	result = m_Device->CreateShaderResourceView(m_RenderTexture, &texShaderDesc, &m_TextureShaderResourceView);
}

void GraphicsAPIWrapper::ConfigureFullScreenRTT(int width, int height)
{
	HRESULT result;

	m_screenwidth = width;
	m_screenheight = height;

	// Render to texture shit
	D3D11_TEXTURE2D_DESC rttdesc;
	ZeroMemory(&rttdesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttdesc.Width = m_screenwidth;
	rttdesc.Height = m_screenheight;
	rttdesc.MipLevels = 1;
	rttdesc.ArraySize = 1;
	rttdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rttdesc.SampleDesc.Count = 1;
	rttdesc.SampleDesc.Quality = 0;
	rttdesc.Usage = D3D11_USAGE_DEFAULT;
	rttdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttdesc.CPUAccessFlags = 0;
	rttdesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the texture
	m_Device->CreateTexture2D(&rttdesc, nullptr, &m_fullscreenTexture);

	// Create render target
	D3D11_RENDER_TARGET_VIEW_DESC RT_DESC;
	ZeroMemory(&RT_DESC, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RT_DESC.Format = rttdesc.Format;
	RT_DESC.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RT_DESC.Texture2D.MipSlice = 0;

	result = m_Device->CreateRenderTargetView(m_fullscreenTexture, &RT_DESC, &m_fullscreenTextureRTV);

	// Create the z buffer texture
	D3D11_TEXTURE2D_DESC rttzdesc;
	ZeroMemory(&rttzdesc, sizeof(D3D11_TEXTURE2D_DESC));
	rttzdesc.ArraySize = 1;
	rttzdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	rttzdesc.Width = rttdesc.Width;
	rttzdesc.Height = rttdesc.Height;
	rttzdesc.Usage = D3D11_USAGE_DEFAULT;
	rttzdesc.Format = DXGI_FORMAT_R32_TYPELESS;
	rttzdesc.MipLevels = 1;
	rttzdesc.SampleDesc.Count = 1;
	rttzdesc.SampleDesc.Quality = 0;
	result = m_Device->CreateTexture2D(&rttzdesc, nullptr, &m_fullscreenTextureZBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC m_defaultDSVD;
	ZeroMemory(&m_defaultDSVD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	m_defaultDSVD.Format = DXGI_FORMAT_D32_FLOAT;
	m_defaultDSVD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_defaultDSVD.Texture2D.MipSlice = 0;

	// Depth stencil state
	D3D11_DEPTH_STENCIL_DESC ddesc;
	ZeroMemory(&ddesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	ddesc.DepthEnable = true;
	ddesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddesc.DepthFunc = D3D11_COMPARISON_LESS;
	ddesc.StencilEnable = false;
	ddesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	result = m_Device->CreateDepthStencilState(&ddesc, &m_DefaultState);

	m_DeviceContext->OMSetDepthStencilState(m_DefaultState, 1);

	ddesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	result = m_Device->CreateDepthStencilState(&ddesc, &m_TransparentState);

	// Create second z buffer
	result = m_Device->CreateDepthStencilView(m_fullscreenTextureZBuffer, &m_defaultDSVD, &m_fullscreenTextureDepthStencil);

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC texShaderDesc;
	ZeroMemory(&texShaderDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	texShaderDesc.Format = rttdesc.Format;
	texShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	texShaderDesc.Texture2D.MostDetailedMip = 0;
	texShaderDesc.Texture2D.MipLevels = 1;

	// Create shader resource view
	result = m_Device->CreateShaderResourceView(m_fullscreenTexture, &texShaderDesc, &m_fullscreenTextureShaderResourceView);

	// Create Shader resource view for z buffer
	texShaderDesc.Format = DXGI_FORMAT_R32_FLOAT;
	result = m_Device->CreateShaderResourceView(m_fullscreenTextureZBuffer, &texShaderDesc, &m_fullscreenTextureZBufferShaderResourceView);
}

void GraphicsAPIWrapper::PrepareForRTT()
{
	ID3D11RenderTargetView* tempRTV[] = { m_fullscreenTextureRTV };

	if (depthBufferEnabled)
		m_DeviceContext->OMSetRenderTargets(1, tempRTV, m_fullscreenTextureDepthStencil); // Depth stencil goes here
	else m_DeviceContext->OMSetRenderTargets(1, tempRTV, nullptr);

	// Clear the back buffer.
	m_DeviceContext->ClearRenderTargetView(m_fullscreenTextureRTV, m_clearScreenColor);
	m_DeviceContext->ClearDepthStencilView(m_fullscreenTextureDepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
}

void GraphicsAPIWrapper::EnableRTTMode()
{
	m_rttmodeEnabled = true;
}

void GraphicsAPIWrapper::DisableRTTMode()
{
	m_rttmodeEnabled = false;
}

bool GraphicsAPIWrapper::RTTModeEnabled()
{
	return m_rttmodeEnabled;
}

void GraphicsAPIWrapper::EnableDepthBuffer()
{
	depthBufferEnabled = true;
	ID3D11RenderTargetView* tempRTV[] = { m_RenderTargetView };
	m_DeviceContext->OMSetRenderTargets(1, tempRTV, m_DepthStencil); // Depth stencil goes here

	m_DeviceContext->ClearDepthStencilView(m_DepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
}

void GraphicsAPIWrapper::DisableDepthBuffer()
{
	depthBufferEnabled = false;
}

void GraphicsAPIWrapper::SetShaderSampler()
{
	ID3D11SamplerState* SAMS[] = { m_fullscreenTextureSampler };
	ID3D11ShaderResourceView* SRVS[] = { m_fullscreenTextureShaderResourceView, m_fullscreenTextureZBufferShaderResourceView };

	m_DeviceContext->PSSetSamplers(0, 1, SAMS);
	m_DeviceContext->PSSetShaderResources(0, 2, SRVS);
}

void GraphicsAPIWrapper::SetOfflineShaderSampler()
{
	m_DeviceContext->PSSetSamplers(0, 1, &m_Sampler);
	m_DeviceContext->PSSetShaderResources(0, 1, &m_TextureShaderResourceView);
}

float GraphicsAPIWrapper::GetRTTAspect()
{
	return m_rtWidth / m_rtHeight;
}

void GraphicsAPIWrapper::EnableBlendState()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0x0;
	m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xfffffff);
	m_DeviceContext->OMSetDepthStencilState(m_TransparentState, 1);
}

void GraphicsAPIWrapper::DisableBlendState()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0x0;
	m_DeviceContext->OMSetBlendState(m_BlendStateDefault, blendFactor, 0xfffffff);
	m_DeviceContext->OMSetDepthStencilState(m_DefaultState, 1);
}

void GraphicsAPIWrapper::EnableRTTDepthBuffer()
{
	depthBufferEnabled = true;
	ID3D11RenderTargetView* tempRTV[] = { m_fullscreenTextureRTV };
	m_DeviceContext->OMSetRenderTargets(1, tempRTV, m_fullscreenTextureDepthStencil); // Depth stencil goes here

	m_DeviceContext->ClearDepthStencilView(m_fullscreenTextureDepthStencil, D3D11_CLEAR_DEPTH, 1, 0);
}

void GraphicsAPIWrapper::DisableRTTDepthBuffer()
{
	m_rttDepthEnabled = false;
}