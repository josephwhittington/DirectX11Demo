#include "Application.h"
#include "Scene.h"
#include "InputManager.h"
#include "Procedural.h"

#define DEG2RADS(deg) ((deg) * (3.1415926)/(180))
#define RADS2DEGS(rads) ((rads) * (180)/(3.1415926))

// Memory leak detection code
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

// Forward declarations
void SetupScenes(GraphicsAPIWrapper* gwrapper, std::vector<Scene*>& scenes, Scene** scene);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//Also need this for memory leak code stuff
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1); //Important!

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize input manager
	InputManager::Initialize();

    // Initialize global strings
	Application* application = new Application;
	if (!application->Initialize(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Set up some scenes
	std::vector<Scene*> scenes;
	Scene* scene;
	SetupScenes(application->GetGraphicsWrapper(), scenes, &scene);

	// Run the application
	application->Run(scenes.data(), scenes.size(), scene);

	// Shutdown the thing
	application->ShutDown();

	// Delete the scenes
	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}
	scenes.clear();

	delete application;

	return (int)0;
}

void moveobject(Scene* scene)
{
	static int thresh = -1;

	std::vector<Model*> models = scene->GetModels();

	XMFLOAT3 pos = models[0]->GetPosition();

	if (pos.x > 10 || pos.x < -10)
		thresh *= -1;
	else pos.x += .1 * thresh;

	models[0]->SetPosition(pos);
}

void SetupScenes(GraphicsAPIWrapper* gwrapper, std::vector<Scene*>& scenes, Scene** scene)
{
	scenes.push_back(new Scene);
	// Setup camera
	FPSCamera* camera = new FPSCamera;
	XMFLOAT3 camPosition(0, 3, -5);
	camera->SetPosition(camPosition);

	// Setup scene
	scenes[0]->SetCamera(camera);

	// Load skybox
	Model* skybox = new Model;
	skybox->SetIsSkybox(true);
	skybox->SetPosition(XMFLOAT3(0, 10, 0));
	skybox->ScaleXYZ(-1, -1, -1);
	skybox->RotateX(180);
	Mesh* skmesh = ResourceManager::LoadWobjectMesh("Assets/models/mapped_skybox.wobj");
	skmesh->SetTextureDiffuseName("SunsetSkybox.dds");
	skybox->InitializeWithMesh(gwrapper, camera, skmesh, "skybox", false, false);
	scenes[0]->SetSkybox(skybox);

	// Load mesh
	Model* tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, -5, -10));
	tempmodel->Initialize(gwrapper, camera, "Assets/models/wood_floor.wobj", "lighting", false, false);
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(-1, 3.6, 0));
	tempmodel->Initialize(gwrapper, camera, "Assets/models/sphere.wobj", "lighting", false, false);
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(2, 2.5, 0));
	skmesh = ResourceManager::LoadWobjectMesh("Assets/models/teapot.wobj");
	skmesh->SetTextureNormalName("SunsetSkybox.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, skmesh, "reflection", false, false);
	scenes[0]->AddModel(tempmodel);
	
	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(10, 15, 0));
	skmesh = Procedural::GenerateQuad(10, "`");
	skmesh->SetTextureNormalName("SunsetSkybox.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, skmesh, "reflection", false, false);
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(10, 10, 0));
	Mesh* blend = ResourceManager::LoadWobjectMesh("Assets/models/mapped_skybox.wobj");
	blend->SetTextureDiffuseName("skybox_3.dds");
	blend->SetTextureNormalName("whittington_logo.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, blend, "texture_blending", false, false);
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(-10, 2, 0));
	blend = Procedural::GenerateQuad(10, "Rock_Diffuse.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, blend, "scrolling_uv", false, false);
	scenes[0]->AddModel(tempmodel);

	// Load the bitch ass Stonehenge
	tempmodel = new Model;
	Mesh* sh = ResourceManager::LoadStoneHenge();
	sh->SetTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "henge", false, true);
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	scenes[0]->AddModel(tempmodel);
	// Load mesh

	tempmodel = new Model;
	sh = ResourceManager::LoadWobjectMesh("Assets/models/bunny.wobj");
	sh->SetTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "henge", false, true);
	tempmodel->ScaleXYZ(100, 100, 100);
	tempmodel->SetPosition(XMFLOAT3(0, 0, 50));
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 10, 0));
	tempmodel->SetTransparency(true);
	tempmodel->RotateX(-90);
	sh = Procedural::GenerateQuad(10, "cloud.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "textured_quad", false, false);
	scenes[0]->AddModel(tempmodel);
	
	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(-10, 10, 0));
	sh = Procedural::GenerateQuad(10, "lari.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "point_to_quad", true, false);
	sh->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	scenes[0]->AddModel(tempmodel);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 20, 0));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempmodel->SetTransparency(true);
	sh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	sh->SetTextureDiffuseName("transparent_blue.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "lighting", false, false);
	scenes[0]->AddModel(tempmodel);
	
	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(5, 20, 0));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempmodel->SetTransparency(true);
	sh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	sh->SetTextureDiffuseName("transparent_red.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "lighting", false, false);
	scenes[0]->AddModel(tempmodel);
	
	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(2.5, 20, 5));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempmodel->SetTransparency(true);
	sh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	sh->SetTextureDiffuseName("transparent_green.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "lighting", false, false);
	scenes[0]->AddModel(tempmodel);

	// Set up directional light
	Light dirLight1;
	ZeroMemory(&dirLight1, sizeof(Light));
	XMFLOAT4 color(1, 1, 1, 1);
	dirLight1.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight1.diffuse = color;
	dirLight1.specular = color;
	dirLight1.ambientIntensity = 0.1f;
	dirLight1.diffuseIntensity = 0.2;
	dirLight1.specularIntensity = 0.4;
	dirLight1.position = XMFLOAT4(0, 0, 10, 1);
	dirLight1.lightDirection = XMFLOAT4(-1, -1, 1, 1);

	// Setup point light
	Light pointLight1;
	ZeroMemory(&pointLight1, sizeof(Light));
	color = XMFLOAT4(0, 1, 0, 1);
	pointLight1.lightType = (unsigned int)LIGHTTYPE::POINT;
	pointLight1.diffuse = color;
	pointLight1.specular = color;
	pointLight1.position = XMFLOAT4(0, 6, 10, 0);
	pointLight1.lightDirection = XMFLOAT4(0, 0, 0, 1);
	// Point light specific variables
	pointLight1.ambientIntensity = 0.1f;
	pointLight1.diffuseIntensity = 0.6;
	pointLight1.specularIntensity = 0.4;
	pointLight1.lightRadius = 30;

	// Setup Spot light
	Light spotLight1;
	ZeroMemory(&spotLight1, sizeof(Light));
	color = XMFLOAT4(0, 0, 1, 1);
	spotLight1.lightType = (unsigned int)LIGHTTYPE::SPOT;
	spotLight1.ambient = color;
	spotLight1.diffuse = color;
	spotLight1.specular = color;
	spotLight1.position = XMFLOAT4(0, 20, 0, 1);
	spotLight1.lightDirection = XMFLOAT4(0, -1, 0, 1);
	// Spotlight Specific variables
	spotLight1.ambientIntensity = 0.1f;
	spotLight1.diffuseIntensity = 1.0;
	spotLight1.specularIntensity = 0.4;
	spotLight1.cosineInnerCone = cosf(DEG2RADS(15));
	spotLight1.cosineOuterCone = cosf(DEG2RADS(20));

	// Add lights to scene
	scenes[0]->AddLight(dirLight1);
	scenes[0]->AddLight(pointLight1);
	scenes[0]->AddLight(spotLight1);

	// Initialize after setting variables
	scenes[0]->Initialize(gwrapper, 0);

	// Second scene
	scenes.push_back(new Scene);
	FPSCamera* cam2 = new FPSCamera;
	camPosition = XMFLOAT3(0, 12, -20);
	cam2->SetPosition(camPosition);

	// Setup scene
	scenes[1]->SetCamera(cam2);

	// Add models
	// Load procedural mesh
	//tempmodel = new Model;
	//sh = Procedural::GenerateQuad(20, "Rock_Diffuse.dds");
	//tempmodel->SetPosition(XMFLOAT3(21, 0, 0));
	//// Set normal map texture
	//sh->SetTextureNormalName("Rock_Normal.dds");
	//tempmodel->InitializeWithMesh(gwrapper, camera, sh, "normal_mapped_terrain");
	//scenes[1]->AddModel(tempmodel);
	
	// Load procedural mesh
	//tempmodel = new Model;
	//sh = Procedural::GenerateQuad(20, "Rock_Diffuse.dds");
	//tempmodel->SetPosition(XMFLOAT3(42, 0, 0));
	//// Set normal map texture
	////sh->SetTextureNormalName("Rock_Normal.dds");
	//tempmodel->InitializeWithMesh(gwrapper, camera, sh, "lighting");
	//scenes[1]->AddModel(tempmodel);

	// Load uv sphere
	/*tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, 10, 15));
	sh = ResourceManager::LoadWobjectMesh("Assets/models/uv_sphere.wobj");
	sh->SetTextureNormalName("Rock_Normal.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "normal_mapped_terrain");
	scenes[1]->AddModel(tempmodel);*/


	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(-10, 2, 0));
	tempmodel->ScaleXYZ(10, 10, 10);
	tempmodel->SetUseTexturedSceneMode(true);
	Mesh* tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	tempMesh->SetTextureDiffuseName("`");
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "lighting", false, false);
	scenes[1]->AddModel(tempmodel);

	skybox = new Model;
	skybox->SetPosition(XMFLOAT3(0, 10, 0));
	skybox->ScaleXYZ(-1, -1, -1);
	skybox->RotateX(180);
	skybox->SetIsSkybox(true);
	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/mapped_skybox.wobj");
	tempMesh->SetTextureDiffuseName("SunsetSkybox.dds");
	skybox->InitializeWithMesh(gwrapper, cam2, tempMesh, "skybox", false, false);
	scenes[1]->SetSkybox(skybox);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(2.5, 20, 5));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempmodel->SetTransparency(true);
	sh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	sh->SetTextureDiffuseName("transparent_green.dds");
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "lighting", false, false);
	scenes[1]->AddModel(tempmodel);
	
	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(20, 0, 0));
	tempMesh = Procedural::GenerateComplexQuad(1, "default_texture.dds");
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "terrain_heightmap", false, false);
	scenes[1]->AddModel(tempmodel);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	// Positions
	std::vector<XMFLOAT3> positions;
	positions.push_back(XMFLOAT3(0, 0, 0));
	positions.push_back(XMFLOAT3(0, 3, 0));
	positions.push_back(XMFLOAT3(0, 6, 0));
	positions.push_back(XMFLOAT3(-3, 0, 0));
	positions.push_back(XMFLOAT3(-3, 3, 0));
	positions.push_back(XMFLOAT3(-3, 6, 0));
	positions.push_back(XMFLOAT3(3, 0, 0));
	positions.push_back(XMFLOAT3(3, 3, 0));
	positions.push_back(XMFLOAT3(3, 6, 0));
	positions.push_back(XMFLOAT3(0, 0, 3));
	positions.push_back(XMFLOAT3(0, 3, 3));
	positions.push_back(XMFLOAT3(0, 6, 3));
	positions.push_back(XMFLOAT3(-3, 0, 3));
	positions.push_back(XMFLOAT3(-3, 3, 3));
	positions.push_back(XMFLOAT3(-3, 6, 3));
	positions.push_back(XMFLOAT3(3, 0, 3));
	positions.push_back(XMFLOAT3(3, 3, 3));
	positions.push_back(XMFLOAT3(3, 6, 3));
	positions.push_back(XMFLOAT3(0, 0, 6));
	positions.push_back(XMFLOAT3(0, 3, 6));
	positions.push_back(XMFLOAT3(0, 6, 6));
	positions.push_back(XMFLOAT3(-3, 0, 6));
	positions.push_back(XMFLOAT3(-3, 3, 6));
	positions.push_back(XMFLOAT3(-3, 6, 6));
	positions.push_back(XMFLOAT3(3, 0, 6));
	positions.push_back(XMFLOAT3(3, 3, 6));
	positions.push_back(XMFLOAT3(3, 6, 6));
	scenes[1]->AddModel(tempmodel);
	tempMesh = ResourceManager::LoadWobjectMeshInstanced("Assets/models/teapot.wobj", positions[0], positions.size());
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "instanced_draw", false, false);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 5, 10));
	tempmodel->RotateX(-90);
	tempMesh = Procedural::GenerateQuad(10, "instancing.dds");
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "textured_quad", false, false);
	scenes[1]->AddModel(tempmodel);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 12, 0));
	tempmodel->Initialize(gwrapper, cam2, "Assets/models/teapot.wobj", "geometry_instancing", true, false);
	scenes[1]->AddModel(tempmodel);

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(10, 10, 0));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/single_mapped_cube.wobj");
	tempMesh->SetTextureDiffuseName("wood_texture.dds");
	tempMesh->SetTextureNormalName("matrix.dds");
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "emission", false, false);
	scenes[1]->AddModel(tempmodel);

	/*tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	tempmodel->RotateY(180);
	tempmodel->ScaleXYZ(5, 5, 5);
	sh = ResourceManager::LoadWobjectMesh("Assets/models/BattleMage.wobj");
	sh->SetTextureNormalName("PPG_3D_Player_N.dds");
;	tempmodel->InitializeWithMesh(gwrapper, cam2, sh, "normal_mapped_terrain");
	scenes[1]->AddModel(tempmodel);*/

	// Lighting
	// Set up directional light
	Light dirLight2;
	ZeroMemory(&dirLight2, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	dirLight2.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight2.diffuse = color;
	dirLight2.specular = color;
	dirLight2.ambientIntensity = 0.1f;
	dirLight2.diffuseIntensity = 1.0;
	dirLight2.specularIntensity = 0.1;
	dirLight2.position = XMFLOAT4(0, 0, 10, 1);
	dirLight2.lightDirection = XMFLOAT4(-1, -1, 1, 1);
	dirLight2.lightOn = true;

	// Setup point light
	Light pointLight2;
	ZeroMemory(&pointLight1, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	pointLight2.lightType = (unsigned int)LIGHTTYPE::POINT;
	pointLight2.diffuse = color;
	pointLight2.specular = color;
	pointLight2.position = XMFLOAT4(0, 5, 0, 0);
	pointLight2.lightDirection = XMFLOAT4(0, 0, 0, 1);
	// Point light specific variables
	pointLight2.ambientIntensity = 0.1f;
	pointLight2.diffuseIntensity = 0.6;
	pointLight2.specularIntensity = 0.1;
	pointLight2.lightRadius = 50;
	pointLight2.lightOn = true;

	scenes[1]->AddLight(dirLight2);
	scenes[1]->AddLight(pointLight2);
	scenes[1]->SetCustomUpdateFunction(moveobject);
	scenes[1]->Initialize(gwrapper, 1);

	scenes.push_back(new Scene);
	FPSCamera* cam3 = new FPSCamera;
	XMFLOAT3 cam3Pos(0, 4.5, -11);
	cam3->SetPosition(cam3Pos);
	scenes[2]->SetCamera(cam3);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	scenes[2]->AddModel(tempmodel);
	tempMesh = ResourceManager::LoadWobjectMeshInstanced("Assets/models/teapot.wobj", positions[0], positions.size());
	tempmodel->InitializeWithMesh(gwrapper, cam2, tempMesh, "instanced_draw", false, false);

	// Lighting
	// Set up directional light
	Light dirLight3;
	ZeroMemory(&dirLight3, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	dirLight3.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight3.diffuse = color;
	dirLight3.specular = color;
	dirLight3.ambientIntensity = 0.1f;
	dirLight3.diffuseIntensity = 1.0;
	dirLight3.specularIntensity = 0.1;
	dirLight3.position = XMFLOAT4(0, 0, 10, 1);
	dirLight3.lightDirection = XMFLOAT4(-1, -1, 1, 1);
	dirLight3.lightOn = true;

	// Setup scene
	scenes[2]->AddLight(dirLight3);
	scenes[2]->Initialize(gwrapper, 2);

	// Bruh
	scenes.push_back(new Scene);
	FPSCamera* cam5 = new FPSCamera;
	XMFLOAT3 cam5Pos(0, 50, -100);
	cam5->Rotate(0, -15);
	cam5->SetPosition(cam5Pos);
	scenes[3]->SetCamera(cam5);

	Light dirLight5;
	ZeroMemory(&dirLight5, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	dirLight5.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight5.diffuse = color;
	dirLight5.specular = color;
	dirLight5.ambientIntensity = 0.1f;
	dirLight5.diffuseIntensity = 1.0;
	dirLight5.specularIntensity = 0.1;
	dirLight5.position = XMFLOAT4(-100, 0, 10, 1);
	dirLight5.lightDirection = XMFLOAT4(-4.9, -1, 1, 1);
	dirLight5.lightOn = true;

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	tempmodel->ScaleXYZ(5, 5, 5);
	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/eh.wobj");
	tempMesh->SetTextureDiffuseName("default_texture.dds");
	tempMesh->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	tempmodel->InitializeWithMesh(gwrapper, cam5, tempMesh, "mountain", false, true);
	scenes[3]->AddModel(tempmodel);
	
	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(0, 50, 0));
	sh = Procedural::GenerateQuad(10, "cloud.dds");
	tempmodel->RotateX(90);
	tempmodel->InitializeWithMesh(gwrapper, camera, sh, "point_to_quad", true, false);
	sh->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	scenes[3]->AddModel(tempmodel);

	scenes[3]->AddLight(dirLight5);
	scenes[3]->Initialize(gwrapper, 3);
	// Bruh

	// Teapot scene
	scenes.push_back(new Scene);
	FPSCamera* cam6 = new FPSCamera;
	XMFLOAT3 cam6Pos(400, 50, -650);
	cam6->SetPosition(cam6Pos);
	scenes[4]->SetCamera(cam6);

	Light dirLight6;
	ZeroMemory(&dirLight6, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	dirLight6.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight6.diffuse = color;
	dirLight6.specular = color;
	dirLight6.ambientIntensity = 0.1f;
	dirLight6.diffuseIntensity = 1.0;
	dirLight6.specularIntensity = 0.1;
	dirLight6.position = XMFLOAT4(0, 0, 10, 1);
	dirLight6.lightDirection = XMFLOAT4(-1, -1, 1, 1);
	dirLight6.lightOn = true;

	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/teapot.wobj");
	std::vector<XMFLOAT3> potpos;

	std::vector<Vertex> VERTS = *(tempMesh->GetVertexDataPointer());
	potpos.resize(tempMesh->GetNumVertices());
	assert(VERTS.size() == potpos.size());
	int shift_amount = 100;
	
	for (int i = 0; i < potpos.size(); i++)
	{
		potpos[i] = XMFLOAT3(VERTS[i].position.x * shift_amount, VERTS[i].position.y * shift_amount, VERTS[i].position.z * shift_amount);
	}
	delete tempMesh;

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(400, 0, 0));
	tempMesh = ResourceManager::LoadWobjectMeshInstanced("Assets/models/teapot_low_poly.wobj", potpos[0], potpos.size());
	tempmodel->InitializeWithMesh(gwrapper, cam5, tempMesh, "instanced_draw", false, false);
	scenes[4]->AddModel(tempmodel);

	// Cup
	VERTS.clear();
	potpos.clear();

	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/teacup.wobj");
	VERTS = *(tempMesh->GetVertexDataPointer());
	potpos.resize(tempMesh->GetNumVertices());
	shift_amount = 2.1;

	for (int i = 0; i < potpos.size(); i++)
	{
		potpos[i] = XMFLOAT3(VERTS[i].position.x * shift_amount, VERTS[i].position.y * shift_amount, VERTS[i].position.z * shift_amount);
	}
	delete tempMesh;

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(200, 0, 0));
	tempmodel->ScaleXYZ(.1, .1, .1);
	tempMesh = ResourceManager::LoadWobjectMeshInstanced("Assets/models/teacup_low_poly.wobj", potpos[0], potpos.size());
	tempmodel->InitializeWithMesh(gwrapper, cam6, tempMesh, "instanced_draw", false, false);
	scenes[4]->AddModel(tempmodel);
	// Cup

	// Cup 2
	VERTS.clear();
	potpos.clear();

	tempMesh = ResourceManager::LoadWobjectMesh("Assets/models/teacup.wobj");
	VERTS = *(tempMesh->GetVertexDataPointer());
	potpos.resize(tempMesh->GetNumVertices());
	shift_amount = 2.1;

	for (int i = 0; i < potpos.size(); i++)
	{
		potpos[i] = XMFLOAT3(VERTS[i].position.x * shift_amount, VERTS[i].position.y * shift_amount, VERTS[i].position.z * shift_amount);
	}
	delete tempMesh;

	tempmodel = new Model();
	tempmodel->SetPosition(XMFLOAT3(600, 0, 0));
	tempmodel->ScaleXYZ(.1, .1, .1);
	tempMesh = ResourceManager::LoadWobjectMeshInstanced("Assets/models/teacup_low_poly.wobj", potpos[0], potpos.size());
	tempmodel->InitializeWithMesh(gwrapper, cam6, tempMesh, "instanced_draw", false, false);
	scenes[4]->AddModel(tempmodel);
	// Cup 2

	// Table
	tempmodel = new Model();
	tempmodel->SetPosition(400, -68, 0);
	tempmodel->ScaleXYZ(210, 120, 120);
	tempmodel->Initialize(gwrapper, cam6, "Assets/models/table.wobj", "lighting", false, false);
	scenes[4]->AddModel(tempmodel);
	// Table

	scenes[4]->AddLight(dirLight6);
	scenes[4]->Initialize(gwrapper, 4);

	// Teapot scene

    // Scenes for full screen quad
	*scene = new Scene;
	FPSCamera* cam4 = new FPSCamera;
	XMFLOAT3 cam4Pos(0, 0, -5);
	cam4->SetPosition(cam4Pos);
	(*scene)->SetCamera(cam4);

	tempmodel = new Model;
	tempmodel->SetPosition(XMFLOAT3(0, 0, 0));
	tempMesh = Procedural::GenerateFullscreenQuad();
	tempmodel->SetRTTEnabledMode(true);
	tempmodel->InitializeWithMesh(gwrapper, cam4, tempMesh, "fullscreen_quad", false, false);
	(*scene)->AddModel(tempmodel);

	// Lighting
	// Set up directional light
	Light dirLight4;
	ZeroMemory(&dirLight4, sizeof(Light));
	color = XMFLOAT4(1, 1, 1, 1);
	dirLight4.lightType = (unsigned int)LIGHTTYPE::DIRECTIONAL;
	dirLight4.diffuse = color;
	dirLight4.specular = color;
	dirLight4.ambientIntensity = 0.1f;
	dirLight4.diffuseIntensity = 1.0;
	dirLight4.specularIntensity = 0.1;
	dirLight4.position = XMFLOAT4(0, 0, 10, 1);
	dirLight4.lightDirection = XMFLOAT4(-1, -1, 1, 1);
	dirLight4.lightOn = true;

	//Setup scene
	(*scene)->AddLight(dirLight4);
	(*scene)->Initialize(gwrapper, -1);

	tempmodel = nullptr;
	tempMesh = nullptr;

	// Sky box cleanup
	skybox = nullptr;
	blend = nullptr;
}
/* NOTE:
 * Every time I add a new light to the light vector I need to manually change 
 * the light count in the pixel shader because DirectX is fucking stoopid
 * Need at least one light per scene
 Take into account that light order can be changed when I'm moving the 
 lights around to check things and this could cause a crash if i do stupid ass shit
 */