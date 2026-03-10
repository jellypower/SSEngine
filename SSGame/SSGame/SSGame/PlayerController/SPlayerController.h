#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

class SPlayerController : public SComponentBase
{
private:
	SGameObject* _CharacterGO = nullptr;

public:
	bool ShouldProcessPerFrameInherently() const override;

	void PostConstructHierarchy() override;

	void PerFrame() override;


public:
	void BindCharacter(SGameObject* InCharacterGO);
};
