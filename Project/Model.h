#pragma once

#define NULLIFYPTR(pointer) (pointer) = nullptr
#define RELEASEPTR(pointer) if ((pointer)) delete pointer

#include <string>
#include <time.h>

#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "GraphicsAPIWrapper.h"

enum class TEXTURE_TYPE
{
	DIFFUSE = 0,
	NORMAL_MAP
};

class Model
{
public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(GraphicsAPIWrapper* gwrapper, Camera* camera, const char* meshname, const char* shadername, bool hasGeometryShader, bool hasTesselationShader);
	bool InitializeWithMesh(GraphicsAPIWrapper* gwrapper, Camera* camera, Mesh* mesh, const char* shadername, bool hasGeometryShader, bool hasTesselationShader);
	void SetPosition(XMFLOAT3 position);
	void SetPosition(float x, float y, float z);
	XMFLOAT3 GetPosition();
	void RotateX(float deg);
	void RotateY(float deg);
	void RotateZ(float deg);
	void ScaleXYZ(float x, float y, float z);
	void Frame(GraphicsAPIWrapper* gwrapper, FPSCamera* camera, double deltaTime, double milliselapsed);

	Mesh* GetMesh();

	void SetDrawInstanced(bool drawInstanced);
	void SetInstancedPositions(XMFLOAT3& startingposition, int numpositions);

	void SetTransparency(bool mode);
	void SetIsSkybox(bool mode);
	bool GetIsSkybox();

	// RTT
	void SetRTTEnabledMode(bool mode);
	void SetUseTexturedSceneMode(bool mode);

private:
	void SetTextures(GraphicsAPIWrapper* gwrapper);

private:
	Mesh* m_Mesh;
	XMFLOAT3 m_position, m_rotation;
	std::vector<Shader*> m_Shaders;
	std::vector<Texture*> m_Textures;

	// Rotations
	float m_xrot, m_yrot, m_zrot;
	// Scale
	float m_xscale, m_yscale, m_zscale;
	// Instance
	bool m_drawInstanced;
	// RTT
	bool m_rttmodeenabled;

	bool m_useTexturedScene;
	bool m_isTransparent;
	bool m_isSkybox;
};