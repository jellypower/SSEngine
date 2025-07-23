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
	CommitCameraRenderInfo();
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

	_FOV = InRadians;

	if (CAM_FOV_MIN <= _FOV && _FOV <= CAM_FOV_MAX &&
		0 < _NearZ &&
		0 < _FarZ &&
		_NearZ < _FarZ)
	{
		Vector2f ViewportSize = g_Renderer->GetViewportSize();
		_ProjMat = XMMatrixPerspectiveFovLH(_FOV, ViewportSize.X / ViewportSize.Y, _NearZ, _FarZ);
		CommitCameraRenderInfo();
	}
}

void SCameraComponent::SetNearZ(float InValue)
{
	_NearZ = InValue;

	if (CAM_FOV_MIN <= _FOV && _FOV <= CAM_FOV_MAX &&
		0 < _NearZ &&
		0 < _FarZ &&
		_NearZ < _FarZ)
	{
		Vector2f ViewportSize = g_Renderer->GetViewportSize();
		_ProjMat = XMMatrixPerspectiveFovLH(_FOV, ViewportSize.X / ViewportSize.Y, _NearZ, _FarZ);
		CommitCameraRenderInfo();
	}
}

void SCameraComponent::SetFarZ(float InValue)
{
	_FarZ = InValue;

	if (CAM_FOV_MIN <=_FOV && _FOV <= CAM_FOV_MAX &&
		0 < _NearZ &&
		0 < _FarZ &&
		_NearZ <_FarZ)
	{
		Vector2f ViewportSize = g_Renderer->GetViewportSize();
		_ProjMat = XMMatrixPerspectiveFovLH(_FOV, ViewportSize.X / ViewportSize.Y, _NearZ, _FarZ);
		CommitCameraRenderInfo();
	}
}

void SCameraComponent::CommitCameraRenderInfo()
{
	const SGameObject* Parent = GetParent();
	const Transform& ParentTransform = Parent->GetTransform();

	XMVECTOR EyePos = ParentTransform.Position.SimdVec;
	XMVECTOR Direction = ParentTransform.GetForward().SimdVec;
	XMVECTOR Up = ParentTransform.GetUp().SimdVec;
	_ViewMat = XMMatrixLookToLH(EyePos, Direction, Up);

	_RenderCamera->SetCameraTransform(ParentTransform);
	_RenderCamera->SetVPMatrix(_ViewMat * _ProjMat);
}