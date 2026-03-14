#include "pch.h"
#include "SPlayerController.h"

#include <SSContentsBase/Public/ContentBase/SWorld.h>
#include <SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h>

#include "SSEngineDefault/Public/RawInput/SSInput.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSGame/Character/SCharacterComponent.h"

bool SPlayerController::ShouldProcessPerFrameInherently() const
{
	return true;
}

void SPlayerController::PostConstructHierarchy()
{
	_PlayerCameraGO = NewSObject<SGameObject>(L"PlayerCamera");
//	_PlayerCameraComp = _PlayerCameraGO->CreateComponent<SCameraComponent>("PlayerCamera");
}

void SPlayerController::OnEnterTheWorld()
{
	SWorld* World = GetIncludedWorld();
	World->AddToWorld(_PlayerCameraGO);
}

void SPlayerController::OnExitTheWorld()
{

}

void SPlayerController::PreDestructHierarchy()
{

}

void SPlayerController::PerFrame(float DeltaTime)
{
	if (SSInput::GetKey(EKeyCode::KEY_A))
	{
		_CharacterComp->AddAccel({-1, 0});
	}
	else if (SSInput::GetKey(EKeyCode::KEY_D))
	{
		_CharacterComp->AddAccel({ 1, 0 });
	}

	if (SSInput::GetKey(EKeyCode::KEY_W))
	{
		_CharacterComp->AddAccel({ 0, 1 });
	}
	else if (SSInput::GetKey(EKeyCode::KEY_S))
	{
		_CharacterComp->AddAccel({ 0, -1 });
	}
}

void SPlayerController::BindCharacter(SGameObject* InCharacterGO)
{
	_CharacterGO = InCharacterGO;
	_CharacterComp = _CharacterGO->FindComponent<SCharacterComponent>();
}
