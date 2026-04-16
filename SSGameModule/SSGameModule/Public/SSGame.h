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
	void SetInGameFocus(bool bFocus) { _bInGameFocus = bFocus; }
	bool IsInGameFocus() const { return _bInGameFocus; }

private:

	void PerFrame_DEBUGDRAW();


private:
	SWorld* _DefaultWorld = nullptr;

	SPlayerController* _MainPalyerController = nullptr;
	SGameObject* _MainCharacter = nullptr;


private:
	bool _bInGameFocus = false;

};
