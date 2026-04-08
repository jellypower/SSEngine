#pragma once


class ICollDevice;
class SPlayerController;
class SCameraComponent;
class IAssetDBLoader;
class IRenderer;
class SSGame;
class SWorld;

extern SSGame* g_Game;

class SSGame : INoncopyable
{
public:
	SSGame(IRenderer* EngineRenderer, ICollDevice* EngineCollDevice);
	virtual ~SSGame();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();


private:
	void StartUpContents();
	void PerFrameContents();


	void MoveFreeCamera();

private:
	SWorld* _DefaultWorld = nullptr;


	bool _bIsFreeCamMode = false;
	SCameraComponent* _FreeCam = nullptr;
	SPlayerController* _MainPalyerController = nullptr;

private:
	IRenderer* _Renderer = nullptr;
	ICollDevice* _CollDevice = nullptr;
	IAssetDBLoader* _AssetDBLoader = nullptr;


private:
	float TEMP_CamXRot = 0;
	float TEMP_CamYRot = 0;
	float TEMP_Speed = 10.f;

};
