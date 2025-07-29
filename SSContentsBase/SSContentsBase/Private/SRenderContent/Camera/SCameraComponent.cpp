#define SSCONTENTBASE_MODULE_EXPORT
#include "SSContentsBase/Public/SRenderContent/Camera/SCameraComponent.h"

#include "SSContentsBase/Public/ContentBase/SGameObject.h"
#include "SSContentsBase/Public/ContentBase/SWorld.h"

#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"


void SCameraComponent::PostConstructHierarchy()
{
	_RenderCamera = g_Renderer->CreateRenderCamera();
}

void SCameraComponent::OnEnterTheWorld()
{
	SGameObject* Parent = GetParent();
	SWorld* IncludedWorld = Parent->GetIncludedWorldRef();
	IRenderWorld* RW = IncludedWorld->GetRenderWorld();

	_RenderCamera->SetIncludedRenderWorldXXX(RW);
}

void SCameraComponent::OnExitTheWorld()
{
	_RenderCamera->SetIncludedRenderWorldXXX(nullptr);
}

void SCameraComponent::PreDestructHierarchy()
{
	delete _RenderCamera;
	_RenderCamera = nullptr;
}

void SCameraComponent::OnGameObjectTransformCommited()
{
	const SGameObject* Parent = GetParent();
	const Transform& ParentTransform = Parent->GetTransform();

	_RenderCamera->SetCameraTransform(ParentTransform);
}

float SCameraComponent::GetNearZ() const
{
	if (_RenderCamera == nullptr)
	{
		SS_ASSERT(false);
		return 0.f;
	}

	return _RenderCamera->GetNearZ();
}

float SCameraComponent::GetFarZ() const
{
	if (_RenderCamera == nullptr)
	{
		SS_ASSERT(false);
		return 0.f;
	}

	return _RenderCamera->GetFarZ();
}

void SCameraComponent::SetAspectRatio(float InRatio)
{
	_RenderCamera->SetAspectRatio(InRatio);
}

void SCameraComponent::SetFOVWithDegrees(float InDegrees)
{
	float Rad = SS::DegToRadians(InDegrees);
	SetFOVWithRadians(Rad);
}

void SCameraComponent::SetFOVWithRadians(float InRadians)
{
	if (InRadians < CAM_FOV_MIN)
	{
		InRadians = CAM_FOV_MIN;
	}
	else if (InRadians > CAM_FOV_MAX)
	{
		InRadians = CAM_FOV_MAX;
	}

	_RenderCamera->SetFOVWithRadians(InRadians);
}

void SCameraComponent::SetNearZ(float InValue)
{
	_RenderCamera->SetNearZ(InValue);
}

void SCameraComponent::SetFarZ(float InValue)
{
	_RenderCamera->SetFarZ(InValue);
}

