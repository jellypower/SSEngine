#pragma once
#include "SSGameModule/ModuleExportKeyword.h"

class SGameObject;
class ICollDevice;
class SPlayerController;
class SCameraComponent;
class IAssetDBLoader;
class IRenderer;
class SSGame;
class SWorld;

extern SSGame* g_Game;

class SSGAME_MODULE SSGame : INoncopyable
{
public:
	SSGame(SWorld* InDefaultWorld);
	virtual ~SSGame();

	void StartUpGame();
	void PerFrameGame();
	void CleanupGame();


public:
	bool IsInGameFocus() const { return _bInGameFocus; }
	void SetInGameFocus(bool bFocus);

	SPlayerController* GetMainPlayerController() const { return _MainPalyerController; }

private:

	void PerFrame_DEBUGDRAW();


private:
	SWorld* _DefaultWorld = nullptr;

	SPlayerController* _MainPalyerController = nullptr;
	SGameObject* _MainCharacter = nullptr;


private:
	bool _bInGameFocus = false;

};
