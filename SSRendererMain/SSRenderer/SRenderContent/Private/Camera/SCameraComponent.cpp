#include "SRenderContent/Public/Camera/SCameraComponent.h"

#include "RenderCamera.h"
#include "SSContentsBase/SGameObject.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"
#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"

void SCameraComponent::PostConstructHierarchy()
{
	_RenderCamera = DBG_NEW RenderCamera(this);
	ConstructRenderTarget();
}

void SCameraComponent::PreDestructHierarchy()
{
	DestructRenderTarget();
	delete _RenderCamera;
	_RenderCamera = nullptr;
}

XMMATRIX SCameraComponent::GetVPMatrix() const
{
	Vector2f WidthHeight;
	if (_RenderTarget != nullptr)
	{
		WidthHeight = _RenderTarget->GetViewportBoxSize().WidthHeight;
	}
	else
	{
		GALRenderDevice* RenderDevice = g_Renderer->_GALRenderDevice;
		GALRenderTarget* RenderTarget = RenderDevice->GetDefaultViewportRenderTarget();
		WidthHeight = RenderTarget->GetViewportBoxSize().WidthHeight;
	}


	XMMATRIX ProjectionMat = XMMatrixPerspectiveFovLH(_FOV, WidthHeight.X / WidthHeight.Y, _NearZ, _FarZ);

	const SGameObject* Parent = GetParent();
	const Transform& ParentTransform = Parent->GetTransform();

	XMVECTOR EyePos = ParentTransform.Position.SimdVec;
	XMVECTOR Direction = ParentTransform.GetForward().SimdVec;
	XMVECTOR Up = ParentTransform.GetUp().SimdVec;
	XMMATRIX ViewMat = XMMatrixLookToLH(EyePos, Direction, Up);

	return ViewMat * ProjectionMat;
}

void SCameraComponent::SetFOVWithRadians(float InRadians)
{
	if (InRadians < CAM_FOV_MIN)
	{
		InRadians = CAM_FOV_MIN;
	}

	if (InRadians > CAM_FOV_MAX)
	{
		InRadians = CAM_FOV_MAX;
	}

	_FOV = InRadians;
}

void SCameraComponent::SetFOVWithDegrees(float InDegrees)
{
	float Rad = SS::DegToRadians(InDegrees);
	SetFOVWithRadians(Rad);
}

void SCameraComponent::ConstructRenderTarget()
{

}

void SCameraComponent::DestructRenderTarget()
{

}
