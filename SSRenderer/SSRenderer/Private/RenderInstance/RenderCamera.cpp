#include "RenderCamera.h"

IRenderWorld* RenderCamera::GetIcludedRenderWorld() const
{
	return _OwnerRenderWorld;
}

const XMMATRIX& RenderCamera::GetVPMatrix() const
{
	return _VPMatrix;
}

const Transform& RenderCamera::GetCameraTransform() const
{
	return _CameraTransform;
}

void RenderCamera::SetVPMatrix(const XMMATRIX& InMatrix)
{
	_VPMatrix = InMatrix;
}

void RenderCamera::SetCameraTransform(const Transform& InTransform)
{
	_CameraTransform = InTransform;
}

void RenderCamera::SetIncludedRenderWorldXXX(IRenderWorld* InWorld)
{
	_OwnerRenderWorld = InWorld;
}
