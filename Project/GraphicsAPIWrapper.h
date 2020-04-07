#pragma once

// Macros
#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()
#define NULLIFYPTR(pointer) (pointer) = nullptr
// Macros

#include <d3d11.h>
#include <DirectXMath.h>

class GraphicsAPIWrapper
{
public:
	GraphicsAPIWrapper();
	GraphicsAPIWrapper(const GraphicsAPIWrapper&);
	~GraphicsAPIWrapper();

	bool Initialize(HWND hWnd, int windowwidth, int windowheight, bool fullscreen, bool vsync);

	void BeginScene();
	void EndScene();

	IDXGISwapChain* GetSwapChain();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	float GetAspectRatio();
	void SetAspectRatio(float aspect);
	void SetFullScreen(bool mode);
	bool GetFullScreen();
	void ShutDown();

	void SetWireframeMode(bool mode);
	bool GetWireframeMode();

	// RTT Shit
	void ConfigureRTT(int width, int height);
	void ConfigureFullScreenRTT(int width, int height);

	void PrepareForRTT();

	void EnableRTTMode();
	void DisableRTTMode();
	bool RTTModeEnabled();

	void EnableDepthBuffer();
	void DisableDepthBuffer();

	void SetShaderSampler();
	void SetOfflineShaderSampler();
	float GetRTTAspect();
	void EnableBlendState();
	void DisableBlendState();

	void EnableRTTDepthBuffer();
	void DisableRTTDepthBuffer();

private:
	// Utility
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	IDXGISwapChain* m_SwapChain;
	// Drawing
	ID3D11Texture2D* m_BackBuffer;
	ID3D11RenderTargetView* m_RenderTargetView;
	D3D11_VIEWPORT m_viewport;
	// Depth buffer
	ID3D11Texture2D* m_ZBuffer;
	ID3D11DepthStencilView* m_DepthStencil;
	// Rasterizer
	ID3D11RasterizerState* m_RasterState;
	ID3D11RasterizerState* m_WireframeRasterState;
	D3D11_RASTERIZER_DESC m_rasterDesc;
	D3D11_BLEND_DESC m_blendDesc;
	ID3D11BlendState* m_BlendState;
	ID3D11BlendState* m_BlendStateDefault;

	float m_clearScreenColor[4];
	float m_aspectRatio;
	bool m_vsyncEnabled, m_fullscreen, depthBufferEnabled;

	// Render to texture shit
	ID3D11Texture2D* m_RenderTexture;
	ID3D11Texture2D* m_RenderTextureZBuffer;

	ID3D11RenderTargetView* m_TextureRTV;
	ID3D11DepthStencilView* m_TextureDepthStencil;

	ID3D11ShaderResourceView* m_TextureShaderResourceView;
	ID3D11SamplerState* m_Sampler;

	// Full screen texture shit
	ID3D11Texture2D* m_fullscreenTexture;
	ID3D11Texture2D* m_fullscreenTextureZBuffer;

	ID3D11RenderTargetView* m_fullscreenTextureRTV;
	ID3D11DepthStencilView* m_fullscreenTextureDepthStencil;

	ID3D11ShaderResourceView* m_fullscreenTextureShaderResourceView;
	ID3D11ShaderResourceView* m_fullscreenTextureZBufferShaderResourceView;
	ID3D11SamplerState* m_fullscreenTextureSampler;

	// Depth stencil
	ID3D11DepthStencilState* m_DefaultState;
	ID3D11DepthStencilState* m_TransparentState;

	int m_rtWidth, m_rtHeight;
	int m_screenwidth, m_screenheight;

	bool m_rttmodeEnabled, m_rttDepthEnabled, m_wireframeModeEnabled;
};