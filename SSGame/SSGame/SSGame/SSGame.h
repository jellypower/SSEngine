#pragma once


class IAssetDBLoader;
class IRenderer;
class SSGame;
class SWorld;

extern SSGame* g_Game;

class SSGame : INoncopyable
{
public:
	SSGame(IRenderer* EngineRenderer);
	virtual ~SSGame();

	void StartupEngine();
	void EnginePerFrame();
	void CleanupEngine();


private:
	void StartUpContents();
	void PerFrameContents();

private:
	SWorld* _DefaultWorld = nullptr;

private:
	IRenderer* _Renderer = nullptr;
	IAssetDBLoader* _AssetDBLoader = nullptr;

};
