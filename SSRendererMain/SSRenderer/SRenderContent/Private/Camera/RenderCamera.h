#pragma once
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"

class SCameraComponent;

class RenderCamera : public IRenderCamera
{
private:
	SCameraComponent* _OwnerCamera = nullptr;

public:
	RenderCamera(SCameraComponent* InOwnerCamera);


	virtual const IRenderWorld* GetIcludedRenderWorld() const override;
	virtual GALRenderTarget* GetSpecificRenderTarget() const override;
	virtual const XMMATRIX& GetVPMatrix() const override;
	virtual const Transform& GetCameraTransform() const override;
};
