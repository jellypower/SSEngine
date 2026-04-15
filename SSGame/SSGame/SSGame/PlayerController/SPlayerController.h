#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SCameraController;
class SCameraComponent;
class SCharacterComponent;

class SPlayerController : public SComponentBase
{
public:
	SCameraController* _PlayerCameraController = nullptr;

private:
	SGameObject* _PlayerCameraGO = nullptr;
	SCameraComponent* _PlayerCameraComp = nullptr;

	SGameObject* _CharacterGO = nullptr;
	SCharacterComponent* _CharacterComp = nullptr;

	float _ControlRotSensitivity = 10.f;

	float _ControlYaw = 0;
	float _ControlPitch = 0;

public:
	SPlayerController();

public:
	bool ShouldProcessPerFrameInherently() const override;
	void PerFrame(float DeltaTime) override;


	void PostConstructHierarchy() override;
	void OnEnterTheWorld() override;
	void OnExitTheWorld() override;
	void PreDestructHierarchy() override;

public:
	const SCameraComponent* GetCameraComp() const;

public:
	void BindCharacter(SGameObject* InCharacterGO);

private:
	void ProcessInput(float DeltaTime);
};

