#pragma once
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"

class RenderCamera : public IRenderCamera
{
public:
	IRenderWorld* GetIcludedRenderWorld() const override;
	const XMMATRIX& GetVPMatrix() const override;
	const Transform& GetCameraTransform() const override;

	void SetVPMatrix(const XMMATRIX& InMatrix) override;
	void SetCameraTransform(const Transform& InTransform) override;

	void SetIncludedRenderWorldXXX(IRenderWorld* InWorld) override;

private:
	IRenderWorld* _OwnerRenderWorld = nullptr;
	XMMATRIX _VPMatrix;
	Transform _CameraTransform;
};
