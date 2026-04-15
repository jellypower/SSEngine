#include "RenderCamera.h"

#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"


constexpr float CAM_FOV_MIN = 0.01f;
constexpr float CAM_FOV_MAX = XM_PI * 0.99f;

IRenderWorld* RenderCamera::GetIcludedRenderWorld() const
{
	return _OwnerRenderWorld;
}

XMMATRIX RenderCamera::CalcVPMatrix() const
{
	if (_OwnerRenderWorld == nullptr)
	{
		SS_ASSERT(false);
		return XMMATRIX();
	}

	XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(_FOV, _AspectRatio, _NearZ, _FarZ);


	XMVECTOR EyePos = _CameraTransform.Position.SimdVec;
	XMVECTOR Direction = _CameraTransform.GetForward().SimdVec;
	XMVECTOR Up = _CameraTransform.GetUp().SimdVec;
	XMMATRIX ViewMat = XMMatrixLookToLH(EyePos, Direction, Up);

	XMMATRIX Result = ViewMat * ProjMat;
	return Result;
}

const Transform& RenderCamera::GetCameraTransform() const
{
	return _CameraTransform;
}

float RenderCamera::GetAspectRatio() const
{
	return _AspectRatio;
}

float RenderCamera::GetFOVWithRadians() const
{
	return _FOV;
}

float RenderCamera::GetNearZ() const
{
	return _NearZ;
}

float RenderCamera::GetFarZ() const
{
	return _FarZ;
}

void RenderCamera::SetAspectRatio(float InRatio)
{
	_AspectRatio = InRatio;
}


void RenderCamera::SetFOVWithDegrees(float InDegrees)
{
	float Rad = SS::DegToRadians(InDegrees);
	SetFOVWithRadians(Rad);
}

void RenderCamera::SetFOVWithRadians(float InRadians)
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
}

void RenderCamera::SetNearZ(float InValue)
{
	_NearZ = InValue;
}

void RenderCamera::SetFarZ(float InValue)
{
	_FarZ = InValue;
}


void RenderCamera::SetCameraTransform(const Transform& InTransform)
{
	_CameraTransform = InTransform;
}

void RenderCamera::SetIncludedRenderWorldXXX(IRenderWorld* InWorld)
{
	_OwnerRenderWorld = InWorld;
}
