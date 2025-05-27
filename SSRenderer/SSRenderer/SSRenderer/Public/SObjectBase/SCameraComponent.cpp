#include "SCameraComponent.h"

#include "SObject/Public/SGameObject.h"
#include "SSGAL/Public/GALRenderTArget/GALRenderTarget.h"

void SCameraComponent::PostConstructHierarchy()
{
	ConstructRenderTarget();
}

void SCameraComponent::PreDestructHierarchy()
{
	DestructRenderTarget();
}

XMMATRIX SCameraComponent::GetVPMatrix() const
{
	const Vector2f& WidthHeight = _RenderTarget->GetViewportBoxSize().WidthHeight;
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
