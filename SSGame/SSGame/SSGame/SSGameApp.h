#pragma once


class SSGame;
class SGameObject;
class ICollDevice;
class SPlayerController;
class SCameraComponent;
class IAssetDBLoader;
class IRenderer;
class SSGameApp;
class SWorld;

extern SSGameApp* g_GameApp;

class SSGameApp : INoncopyable
{
public:
	SSGameApp(IRenderer* EngineRenderer, ICollDevice* EngineCollDevice);
	virtual ~SSGameApp();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();


private:
	void MoveFreeCamera();

private:
	SWorld* _DefaultWorld = nullptr;
	SSGame* _Game = nullptr;

	bool _bIsFreeCamMode = false;
	SCameraComponent* _FreeCam = nullptr;

private:
	IRenderer* _Renderer = nullptr;
	ICollDevice* _CollDevice = nullptr;
	IAssetDBLoader* _AssetDBLoader = nullptr;


private:
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;

};
