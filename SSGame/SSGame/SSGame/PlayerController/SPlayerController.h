#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SCameraComponent;
class SCharacterComponent;

class SPlayerController : public SComponentBase
{
private:
	SGameObject* _PlayerCameraGO = nullptr;
	SCameraComponent* _PlayerCameraComp = nullptr;

	SGameObject* _CharacterGO = nullptr;
	SCharacterComponent* _CharacterComp = nullptr;

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;


	void PostConstructHierarchy() override;
	void OnEnterTheWorld() override;
	void OnExitTheWorld() override;
	void PreDestructHierarchy() override;

public:
	void BindCharacter(SGameObject* InCharacterGO);
};
