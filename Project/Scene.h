#pragma once

#define NULLIFYPTR(pointer) (pointer) = nullptr
#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()
#define RELEASEPTR(pointer) if ((pointer)) delete pointer

#include "Effects.h"
#include "Model.h"
#include "Camera.h"
#include "InputManager.h"
#include "Light.h"

class Scene
{
public:
	Scene();
	Scene(const Scene&);
	~Scene();

	void Frame(GraphicsAPIWrapper* gwrapper, EFFECT_DATA& effect_data, double deltaTime, double milliselapsed);
	void HandleInput();

	void AddModel(Model* model);
	void SetSkybox(Model* model);
	void AddLight(Light& light);
	void SetCamera(FPSCamera* camera);
	Camera* GetCamera();
	void Initialize(GraphicsAPIWrapper* gwrapper, int sceneindex);

	std::vector<Model*> GetModels();

	void SetCustomUpdateFunction(void (*update)(Scene* scene));

private:
	void (*update)(Scene* scene);

private:
	ID3D11Buffer* m_LightBuffer;
	ID3D11Buffer* m_EffectBuffer;
	ID3D11Buffer* m_TessBuffer;
	FPSCamera* m_Camera;
	float thresh, zoomThresh, rotationThresh;
	float tesstimer;
	int lightcount, m_tesslevel;
	Model* skybox;
	std::vector<Model*> m_Models;
	std::vector<Light> m_lights;

	int sceneIndex;
	float xlimit;
};
// Note: Run initialize after creating light sources because the buffer is fucked otherwise