#include "InputManager.h"

bool InputManager::m_keys[] = { false };
int InputManager::m_mouseButtons[] = { false };

// Mouse stuff
int InputManager::m_mouseXDelta = 0;
int InputManager::m_mouseYDelta = 0;
int InputManager::m_lastX = 0;
int InputManager::m_lastY = 0;

void InputManager::Initialize()
{
	for (int i = 0; i < 256; i++)
		InputManager::m_keys[i] = false;

	for (int i = 0; i < 3; i++)
		InputManager::m_mouseButtons[i] = false;
}

void InputManager::SetKeyUp(unsigned int key)
{
	InputManager::m_keys[key] = false;
}

void InputManager::SetKeyDown(unsigned int key)
{
	InputManager::m_keys[key] = true;
}

void InputManager::SetMouseButtonUp(MOUSE_BUTTONS btn)
{
	InputManager::m_mouseButtons[(int)btn] = false;
}

void InputManager::SetMouseButtonDown(MOUSE_BUTTONS btn)
{
	InputManager::m_mouseButtons[(int)btn] = true;
}

bool InputManager::IsKeyUp(unsigned int key)
{
	return !InputManager::m_keys[key];
}

bool InputManager::IsKeyUp(BUTTONS key)
{
	return !InputManager::m_keys[(unsigned int)key];
}

bool InputManager::IsKeyDown(unsigned int key)
{
	return InputManager::m_keys[key];
}

bool InputManager::IsKeyDown(BUTTONS key)
{
	return InputManager::m_keys[(unsigned int)key];
}

bool InputManager::IsMouseButtonUp(MOUSE_BUTTONS btn)
{
	return !InputManager::m_mouseButtons[(int)btn];
}

bool InputManager::IsMouseButtonDown(MOUSE_BUTTONS btn)
{
	return InputManager::m_mouseButtons[(int)btn];
}

void InputManager::SetMouseDeltas(int xchange, int ychange)
{
	InputManager::m_mouseXDelta += xchange;
	InputManager::m_mouseYDelta += ychange;
}

int InputManager::GetMouseXDelta()
{
	m_mouseXDelta = 0;

	return InputManager::m_mouseXDelta;
}

int InputManager::GetMouseYDelta()
{
	m_mouseYDelta = 0;

	return InputManager::m_mouseYDelta;
}