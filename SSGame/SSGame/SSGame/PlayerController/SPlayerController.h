#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SCharacterComponent;

class SPlayerController : public SComponentBase
{
private:
	SGameObject* _CharacterGO = nullptr;
	SCharacterComponent* _CharacterComp = nullptr;

public:
	bool ShouldProcessPerFrameInherently() const override;

	void PostConstructHierarchy() override;

	void PerFrame(float DeltaTime) override;


public:
	void BindCharacter(SGameObject* InCharacterGO);
};
