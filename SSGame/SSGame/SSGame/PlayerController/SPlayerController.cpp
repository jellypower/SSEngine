#include "pch.h"
#include "SPlayerController.h"

bool SPlayerController::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SPlayerController::PostConstructHierarchy()
{
	
}

void SPlayerController::PerFrame()
{
}

void SPlayerController::BindCharacter(SGameObject* InCharacterGO)
{
	_CharacterGO = InCharacterGO;
}
