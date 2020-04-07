#include "Application.h"
#include "assets/StoneHenge.h"

Application::Application()
{
	NULLIFYPTR(m_constantBuffer);

	m_primarySceneIndex = 0;
	m_secondarySceneIndex = 0;

	m_lastSceneChange = 0;

	m_effectData.effectid = 0;
}

Application::Application(const Application&)
{

}

Application::~Application()
{

}

bool Application::Initialize(HINSTANCE hin, int nCmdShow)
{
	m_hInstance = hin;
	m_nCmdShow = nCmdShow;

	// Set application name
	m_applicationName = L"Whittington's Project";
	m_appNameCstr = "Whittington's Project";

	// Set default screen dimensions as a safeguard
	m_screenHeight = m_screenWidth = 0;

	// Set the default screen size
	m_windowWidth = 1024; m_windowHeight = 768;
	InitializeWindows();

	// Reset the aspect ratio for the window
	RECT window;
	GetClientRect(m_hWnd, &window);

	int width = window.right - window.left;
	int height = window.bottom - window.top;

	// Initialize Graphics Wrapper
	m_gWrapper = new GraphicsAPIWrapper;



	if (!m_gWrapper->Initialize(m_hWnd, width, height, false, false))
	{
		return false;
	}

	std::chrono::high_resolution_clock::time_point timepoint = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration duration = timepoint.time_since_epoch();
	m_startTime = duration.count();
	m_lastTime = m_startTime;
	m_milliElapsed = 0;
	m_deltaTime = 0;

	return true;
}

void Application::Run(Scene** src, int numscenes, Scene* quadScene)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Set the scenes
	m_outputQuad = quadScene;

	m_Scenes.resize(numscenes);
	memcpy(m_Scenes.data(), src, numscenes * sizeof(Scene*));

	// Main message loop:
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) break;

		if (InputManager::IsKeyDown(VK_ESCAPE))
		{
			if (m_gWrapper->GetFullScreen())
				ToggleFullscreen();

			break;
		}

		// Draw 
		if(!Frame()) break;
		// Draw
	}
}

void Application::ShutDown()
{
	if (m_constantBuffer) 
		m_constantBuffer->Release();

	m_gWrapper->ShutDown();
	delete m_gWrapper;

	delete m_outputQuad;
	m_outputQuad = nullptr;
}

ID3D11Device* Application::GetGraphicsDevice()
{
	return m_gWrapper->GetDevice();
}

GraphicsAPIWrapper* Application::GetGraphicsWrapper()
{
	return m_gWrapper;
}

std::vector<Scene*>* Application::GetScenes()
{
	return &m_Scenes;
}

void Application::ToggleFullscreen()
{
	if (!m_gWrapper->GetFullScreen())
	{
		

		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);

		m_gWrapper->SetFullScreen(true);
		m_gWrapper->SetAspectRatio((float)w / h);
	}
	else
	{
		m_gWrapper->SetFullScreen(false);
		m_gWrapper->SetAspectRatio((float)m_windowWidth / m_windowHeight);

		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, NULL, (m_screenWidth - m_windowWidth) / 2, (m_screenHeight - m_windowHeight) / 2, m_windowWidth, m_windowHeight, SWP_FRAMECHANGED);
	}
}

bool Application::Frame()
{
	TimeTick();
	HandleInput();

	// Rtt pass
	m_gWrapper->EnableRTTMode();
	m_gWrapper->BeginScene();
	if (m_Scenes[2])
		m_Scenes[2]->Frame(m_gWrapper, m_effectData, m_deltaTime, m_milliElapsed);
	m_gWrapper->DisableRTTMode();
	m_gWrapper->SetOfflineShaderSampler();

	// First Render pass
	m_gWrapper->PrepareForRTT();
	if (m_Scenes[m_primarySceneIndex])
		m_Scenes[m_primarySceneIndex]->Frame(m_gWrapper, m_effectData, m_deltaTime, m_milliElapsed);

	// Real render
	m_gWrapper->BeginScene();
	if (m_Scenes[m_primarySceneIndex])
		m_outputQuad->Frame(m_gWrapper,m_effectData, m_deltaTime, m_milliElapsed);
	m_gWrapper->EndScene();

	return true;
}

void Application::TimeTick()
{
	std::chrono::high_resolution_clock::time_point timepoint = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::duration duration = timepoint.time_since_epoch();

	double currenttime = duration.count();
	m_deltaTime = (currenttime - m_lastTime) / 1000000000; // 10^9
	m_milliElapsed = (currenttime - m_startTime) / 1000000; // 10^6
	m_lastTime = currenttime;

	m_lastSceneChange += m_deltaTime;

	toggletime += m_deltaTime;
}

void Application::HandleInput()
{
	if (InputManager::IsKeyDown(VK_SHIFT) && !InputManager::IsKeyDown(VK_CONTROL))
	{
		// If the user holds shift and presses a number and that scene exists
		// Make that the new active scene
		if (InputManager::IsKeyDown(InputManager::BUTTONS::NUM_ZERO))
		{
			// Always allow to revert to the scene
			m_primarySceneIndex = 0;
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::NUM_ONE))
		{
			// If a second scene exists make that the primary scene and initialize it
			if (m_Scenes.size() - 1 >= 1)
			{
				m_primarySceneIndex = 1;
			}
		}

		// Left/ right arrows will cycle through scenes
		if (InputManager::IsKeyDown(VK_LEFT) && m_lastSceneChange > .2)
		{
			m_primarySceneIndex -= 1;
			if (m_primarySceneIndex < 0) m_primarySceneIndex = m_Scenes.size() - 1;
			m_lastSceneChange = 0;
		}
		else if (InputManager::IsKeyDown(VK_RIGHT) && m_lastSceneChange > .2)
		{
			m_primarySceneIndex += 1;
			if (m_primarySceneIndex >= m_Scenes.size()) m_primarySceneIndex = 0;
			m_lastSceneChange = 0;
		}
	}
	else if (InputManager::IsKeyDown(VK_CONTROL))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_F) & toggletime > .2)
		{
			toggletime = 0;

			ToggleFullscreen();
		}

		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_B) && toggletime > .2)
		{
			toggletime = 0;
			m_gWrapper->SetWireframeMode(!m_gWrapper->GetWireframeMode());
		}

		if (InputManager::IsKeyDown(VK_SHIFT) && InputManager::IsKeyDown(VK_SPACE) && toggletime > .2)
		{
			toggletime = 0;

			m_effectData.effectid -= 1;
			if (m_effectData.effectid < 0) m_effectData.effectid = 5;
		}
		else if (InputManager::IsKeyDown(VK_SPACE) && toggletime > .2)
		{
			toggletime = 0;

			m_effectData.effectid += 1;
			if (m_effectData.effectid > 5) m_effectData.effectid = 0;
		}
	}
}

void Application::InitializeWindows()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_applicationName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_WINLOGO);

	RegisterClassExW(&wcex);

	// Determine the resolution of the clients desktop screen.
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_appNameCstr,
		m_appNameCstr,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		(m_screenWidth - m_windowWidth)/2,
		(m_screenHeight - m_windowHeight)/2,
		m_windowWidth,
		m_windowHeight,
		nullptr,
		nullptr,
		m_hInstance,
		nullptr
	);

	ShowWindow(m_hWnd, m_nCmdShow);
	UpdateWindow(m_hWnd);
}

void Application::ShutDownWindows()
{
	delete handle;
	handle = nullptr;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	unsigned int thing;
	switch (message)
	{
	case WM_KEYDOWN:
	{
		thing = (unsigned int)wParam;
		InputManager::SetKeyDown((unsigned int)(wParam));
		break;
	}
	case WM_KEYUP:
		thing = (unsigned int)wParam;
		InputManager::SetKeyUp((unsigned int)(wParam));
		break;
	case WM_LBUTTONDOWN:
		InputManager::SetMouseButtonDown(InputManager::MOUSE_BUTTONS::LEFT_MOUSE_BUTTON);
		break;
	case WM_LBUTTONUP:
		InputManager::SetMouseButtonUp(InputManager::MOUSE_BUTTONS::LEFT_MOUSE_BUTTON);
		break;
	case WM_MBUTTONDOWN:
		InputManager::SetMouseButtonDown(InputManager::MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON);
		break;
	case WM_MBUTTONUP:
		InputManager::SetMouseButtonUp(InputManager::MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON);
		break;
	case WM_RBUTTONDOWN:
		InputManager::SetMouseButtonDown(InputManager::MOUSE_BUTTONS::RIGHT_MOUSE_BUTTON);
		break;
	case WM_RBUTTONUP:
		InputManager::SetMouseButtonUp(InputManager::MOUSE_BUTTONS::RIGHT_MOUSE_BUTTON);
		break;
	case WM_SIZE:
	{
		// Reset the aspect ratio for the window
		RECT window;
		GetClientRect(hWnd, &window);

		int width = window.right - window.left;
		int height = window.bottom - window.top;

		// Resize window and set aspect ratio here
		break;
	}
	case WM_MOUSEMOVE:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		InputManager::SetMouseDeltas(xPos, yPos);

		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case WM_QUIT:
		{
			if (handle->GetGraphicsWrapper()->GetFullScreen())
			{
				handle->ToggleFullscreen();
			}
			DestroyWindow(hWnd);
			break;
		}
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}