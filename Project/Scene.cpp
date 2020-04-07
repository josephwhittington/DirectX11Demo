#include "Scene.h"

int lightdirection = 1;

Scene::Scene()
{
	thresh = 0.1;
	zoomThresh = 0.1;
	NULLIFYPTR(m_Camera);
	NULLIFYPTR(m_LightBuffer);
	NULLIFYPTR(m_EffectBuffer);
	NULLIFYPTR(m_TessBuffer);
	NULLIFYPTR(skybox);

	lightcount = 0;
	m_lights.resize(16);
	for (int i = 0; i < m_lights.size(); i++)
	{
		m_lights[0].lightType = 100;
	}

	update = nullptr;

	tesstimer = 0;
	m_tesslevel = 1;
}

Scene::Scene(const Scene&)
{

}

Scene::~Scene()
{
	RELEASEPTR(m_Camera);
	RELEASEPTR(skybox);
	D3DRELEASE(m_LightBuffer);
	D3DRELEASE(m_TessBuffer);
	D3DRELEASE(m_EffectBuffer);

	for (int i = 0; i < m_Models.size(); i++)
	{
		delete m_Models[i];
	}
}


void Scene::Frame(GraphicsAPIWrapper* gwrapper, EFFECT_DATA& effect_data, double deltaTime, double milliselapsed)
{
	// Timers and shit
	tesstimer += deltaTime;

	if(!gwrapper->RTTModeEnabled())
		HandleInput();

	if(update)
		update(this);

	// Billboarded cloud shit
	static float thresh = 0.01;
	if (sceneIndex == 3)
	{
		XMFLOAT3 p = m_Models[1]->GetPosition();
		p.x -= thresh;
		if (p.x < -100)
			p.x = 100;
		m_Models[1]->SetPosition(p);
	}
	// Billboarded cloud shit

	float lightthresh = .01;
	if (InputManager::IsKeyDown(VK_SHIFT))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_J))
		{
			if (m_lights[1].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[1].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[1].position.x -= lightthresh;
			}
			//m_lights[2].position.x -= lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_L))
		{
			if (m_lights[1].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[1].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[1].position.x += lightthresh;
			}
			//m_lights[2].position.x += lightthresh;
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_I))
		{
			if (m_lights[1].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[1].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[1].position.z += lightthresh;
			}
			//m_lights[2].position.y += lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_K))
		{
			if (m_lights[1].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[1].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[1].position.z -= lightthresh;
			}
			//m_lights[2].position.y -= lightthresh;
		}
	} else if (InputManager::IsKeyDown(VK_CONTROL))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_J))
		{
			if (m_lights[2].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[2].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[2].position.x -= lightthresh;
			}
			//m_lights[2].position.x -= lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_L))
		{
			if (m_lights[2].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[2].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[2].position.x += lightthresh;
			}
			//m_lights[2].position.x += lightthresh;
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_I))
		{
			if (m_lights[2].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[2].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[2].position.z += lightthresh;
			}
			//m_lights[2].position.y += lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_K))
		{
			if (m_lights[2].lightType == (unsigned int)LIGHTTYPE::POINT || m_lights[2].lightType == (unsigned int)LIGHTTYPE::SPOT)
			{
				m_lights[2].position.z -= lightthresh;
			}
			//m_lights[2].position.y -= lightthresh;
		}
	}
	else
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_I))
		{
			m_lights[0].lightDirection.z += lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_K))
		{
			m_lights[0].lightDirection.z -= lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_J))
		{
			m_lights[0].lightDirection.x += lightthresh;
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_L))
		{
			m_lights[0].lightDirection.x -= lightthresh;
		}
	}
	
	// Light position

	// Light buffer
	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	HRESULT result = gwrapper->GetDeviceContext()->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
	int buffersize = sizeof(Light) * m_lights.size();
	memcpy(gpuBuffer.pData, m_lights.data(), buffersize);
	gwrapper->GetDeviceContext()->Unmap(m_LightBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE effectBuffer;
	result = gwrapper->GetDeviceContext()->Map(m_EffectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &effectBuffer);
	buffersize = sizeof(EFFECT_DATA);
	memcpy(effectBuffer.pData, &effect_data, buffersize);
	gwrapper->GetDeviceContext()->Unmap(m_EffectBuffer, 0);

	// Connect constant buffer to the pipeline
	ID3D11Buffer* buffer[] = { m_LightBuffer, m_EffectBuffer };
	gwrapper->GetDeviceContext()->PSSetConstantBuffers(0, 2, buffer);
	// Light buffer

	// Tess buffer
	D3D11_MAPPED_SUBRESOURCE tessBuffer;
	result = gwrapper->GetDeviceContext()->Map(m_TessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tessBuffer);
	buffersize = sizeof(TESS_DATA);
	memcpy(tessBuffer.pData, &m_tesslevel, buffersize);
	gwrapper->GetDeviceContext()->Unmap(m_TessBuffer, 0);

	ID3D11Buffer* tessbuffers[] = { m_TessBuffer };
	gwrapper->GetDeviceContext()->HSSetConstantBuffers(0, 1, tessbuffers);
	// Tess buffer

	// Loop over the models and render them hoes
	// Draw the skybox
	if (skybox && !gwrapper->GetWireframeMode())
	{
		// Set the skybox position to surround the camera
		skybox->SetPosition(m_Camera->GetPosition());

		gwrapper->DisableRTTDepthBuffer();
		skybox->Frame(gwrapper, m_Camera, deltaTime, milliselapsed);
		gwrapper->EnableRTTDepthBuffer();
	}

	for (int i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->Frame(gwrapper, m_Camera, deltaTime, milliselapsed);
	}
}

void Scene::HandleInput()
{
	if (InputManager::IsMouseButtonUp(InputManager::MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON) && InputManager::IsMouseButtonUp(InputManager::MOUSE_BUTTONS::LEFT_MOUSE_BUTTON) && InputManager::IsKeyUp(VK_SHIFT))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_W))
		{
			XMFLOAT3 forward = m_Camera->GetLook();
			forward.x *= thresh;
			forward.y *= thresh;
			forward.z *= thresh;
			m_Camera->Move(forward);
		}

		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_S))
		{
			XMFLOAT3 forward = m_Camera->GetLook();
			forward.x *= -thresh;
			forward.y *= -thresh;
			forward.z *= -thresh;
			m_Camera->Move(forward);
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_A))
		{
			XMFLOAT3 right = m_Camera->GetRight();
			right.x *= thresh;
			right.y *= thresh;
			right.z *= thresh;
			m_Camera->Move(right);
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_D))
		{
			XMFLOAT3 left = m_Camera->GetRight();
			left.x *= -thresh;
			left.y *= -thresh;
			left.z *= -thresh;
			m_Camera->Move(left);
		}
	}
	else if(InputManager::IsMouseButtonDown(InputManager::MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_W))
		{
			m_Camera->Rotate(0, XMConvertToRadians(10));
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_S))
		{
			m_Camera->Rotate(0, XMConvertToRadians(-10));
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_A))
		{
			m_Camera->Rotate(XMConvertToRadians(-10), 0);
		}
		else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_D))
		{
			m_Camera->Rotate(XMConvertToRadians(10), 0);
		}
	}
	else if (InputManager::IsKeyDown(VK_SHIFT))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_W))
		{
			XMFLOAT3 forward = XMFLOAT3(0, 0, thresh);
			m_Camera->Move(forward);
		}

		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_S))
		{
			XMFLOAT3 backward = XMFLOAT3(0, 0, -thresh);;
			m_Camera->Move(backward);
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_A))
		{
			XMFLOAT3 right = XMFLOAT3(thresh, 0, 0);
			m_Camera->Move(right);
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_D))
		{
			XMFLOAT3 left = XMFLOAT3(-thresh, 0, 0);
			m_Camera->Move(left);
		}
	}

	if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_N))
	{
		if (InputManager::IsKeyDown(VK_UP))
		{
			float _near = m_Camera->GetNear() + thresh;
			if (_near < 0.1) _near = .1;
			else if (_near > 20) _near = 5;
			m_Camera->SetClippingPlanes(_near, m_Camera->GetFar());
		}
		if (InputManager::IsKeyDown(VK_DOWN))
		{
			float _near = m_Camera->GetNear() - thresh;
			if (_near < 0.1) _near = .1;
			else if (_near > 20) _near = 5;
			m_Camera->SetClippingPlanes(_near, m_Camera->GetFar());
		}
	}
	else if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_F))
	{
		if (InputManager::IsKeyDown(VK_UP))
		{
			float _far = m_Camera->GetFar() + thresh;
			if (_far > 1000) _far = 1000;
			else if (_far < 60) _far = 60;
			m_Camera->SetClippingPlanes(m_Camera->GetNear(), _far);
		}
		if (InputManager::IsKeyDown(VK_DOWN))
		{
			float _far = m_Camera->GetFar() - thresh;
			if (_far > 1000) _far = 1000;
			else if (_far < 60) _far = 60;
			m_Camera->SetClippingPlanes(m_Camera->GetNear(), _far);
		}
	}
	else
	{
		/*if (InputManager::IsKeyDown(VK_UP))
		{
			XMFLOAT3 pos(0, thresh, 0);
				m_Camera->Move(pos);
		}
		if (InputManager::IsKeyDown(VK_DOWN))
		{
			XMFLOAT3 pos(0, -thresh, 0);
			m_Camera->Move(pos);
		}*/
	}
	if (InputManager::IsMouseButtonDown(InputManager::MOUSE_BUTTONS::LEFT_MOUSE_BUTTON))
	{
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_W))
		{
			// Decrease the fov up to 60 degrees
			float fovdegrees = m_Camera->GetFOVDegrees() - zoomThresh;
			// Clamp value
			if (fovdegrees >= 60)
				m_Camera->SetFOV(fovdegrees);
		}
		if (InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_S))
		{
			// Increase the fov up to 120 degrees
			float fovdegrees = m_Camera->GetFOVDegrees() + zoomThresh;
			// Clamp value
			if (fovdegrees <= 120)
				m_Camera->SetFOV(fovdegrees);
		}
	}

	if (!InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_N) && !InputManager::IsKeyDown(InputManager::BUTTONS::LETTER_F) && !InputManager::IsKeyDown(VK_SHIFT))
	{
		if (InputManager::IsKeyDown(VK_UP))
		{
			m_Camera->Rotate(0, XMConvertToRadians(10));
		}
		else if (InputManager::IsKeyDown(VK_DOWN))
		{
			m_Camera->Rotate(0, XMConvertToRadians(-10));
		}
		if (InputManager::IsKeyDown(VK_RIGHT))
		{
			m_Camera->Rotate(XMConvertToRadians(-10), 0);
		}
		else if (InputManager::IsKeyDown(VK_LEFT))
		{
			m_Camera->Rotate(XMConvertToRadians(10), 0);
		}
	}
	else if (InputManager::IsKeyDown(VK_SHIFT))
	{
		if (InputManager::IsKeyDown(VK_UP))
		{
			XMFLOAT3 pos(0, thresh, 0);
			m_Camera->Move(pos);
		}
		if (InputManager::IsKeyDown(VK_DOWN))
		{
			XMFLOAT3 pos(0, -thresh, 0);
			m_Camera->Move(pos);
		}
	}

	if (InputManager::IsKeyDown(VK_OEM_4) && tesstimer > .2)
	{
		tesstimer = 0;
		m_tesslevel -= 1;

		if (m_tesslevel < 0)
			m_tesslevel = 0;
	}
	else if (InputManager::IsKeyDown(VK_OEM_6) && tesstimer > .2)
	{
		tesstimer = 0;
		m_tesslevel += 1;

		if (m_tesslevel > 64)
			m_tesslevel = 64;
	}

	//// mOUSE MOVEMENT
	//int xdelta = InputManager::GetMouseXDelta();
	//int ydelta = InputManager::GetMouseYDelta();
	//m_Camera->Rotate(XMConvertToRadians(((float)xdelta / 1080)* thresh), XMConvertToRadians(((float)ydelta / 728)* thresh));
}

void Scene::AddModel(Model* model)
{
	m_Models.push_back(model);
}

void Scene::SetSkybox(Model* model)
{
	skybox = model;
}

void Scene::AddLight(Light& light)
{
	m_lights[lightcount] = light;
	lightcount++;
}

void Scene::SetCamera(FPSCamera* camera)
{
	m_Camera = camera;
}

Camera* Scene::GetCamera()
{
	return m_Camera;
}

void Scene::Initialize(GraphicsAPIWrapper* gwrapper, int sceneindex)
{
	sceneIndex = sceneindex;

	// Create constant buffer
	D3D11_BUFFER_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));

	// Byte width
	int bytewidth = sizeof(Light) * m_lights.size();
	// If the addition is not needed this evaluates to 0
	bytewidth += (bytewidth % 16 != 0) * (16 - (bytewidth % 16));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = bytewidth;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT result = gwrapper->GetDevice()->CreateBuffer(&bDesc, nullptr, &m_LightBuffer);
	assert(!FAILED(result));
	// Byte width
	bytewidth = sizeof(EFFECT_DATA);
	// If the addition is not needed this evaluates to 0
	bytewidth += (bytewidth % 16 != 0) * (16 - (bytewidth % 16));
	bDesc.ByteWidth = bytewidth;
	result = gwrapper->GetDevice()->CreateBuffer(&bDesc, nullptr, &m_EffectBuffer);
	assert(!FAILED(result));

	// Bytewidth
	bytewidth = sizeof(TESS_DATA);
	// If the addition is not needed this evaluates to 0
	bytewidth += (bytewidth % 16 != 0) * (16 - (bytewidth % 16));
	bDesc.ByteWidth = bytewidth;
	result = gwrapper->GetDevice()->CreateBuffer(&bDesc, nullptr, &m_TessBuffer);
	assert(!FAILED(result));
}

std::vector<Model*> Scene::GetModels()
{
	return m_Models;
}

void Scene::SetCustomUpdateFunction(void (*_update)(Scene* scene))
{
	update = _update;
}