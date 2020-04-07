#pragma once

#include "framework.h"
#include "Effects.h"
#include <windowsx.h>
#include <chrono>
#include "GraphicsAPIWrapper.h"
#include "Scene.h"
#include "InputManager.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	void Run(Scene** src, int numscenes, Scene* quadScene);
	void ShutDown();


	ID3D11Device* GetGraphicsDevice();
	GraphicsAPIWrapper* GetGraphicsWrapper();
	std::vector<Scene*>* GetScenes();

	// Screen shit
	void ToggleFullscreen();

private:
	bool Frame();
	void TimeTick();
	void HandleInput();
	void InitializeWindows();
	void ShutDownWindows();

private:
	std::vector<Scene*> m_Scenes;
	std::vector<Scene*> m_RttScenes;

	ID3D11Buffer* m_constantBuffer;
	Scene* m_outputQuad;
	EFFECT_DATA m_effectData;

	LPCWSTR m_applicationName;
	LPCSTR m_appNameCstr;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nCmdShow;

	GraphicsAPIWrapper* m_gWrapper;

	int m_windowWidth, m_windowHeight;
	int m_screenWidth, m_screenHeight;

	double m_startTime, m_lastTime, m_milliElapsed, m_deltaTime, m_lastSceneChange;

	int m_primarySceneIndex, m_secondarySceneIndex;

	float toggletime = 0;
};

static Application* handle = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);