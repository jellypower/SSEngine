#pragma once
#include "SSRenderer/Public/RenderInstance/IRenderCamera.h"

class RenderCamera : public IRenderCamera
{
public:
	IRenderWorld* GetIcludedRenderWorld() const override;
	XMMATRIX CalcVPMatrix() const override;
	const Transform& GetCameraTransform() const override;

	virtual float GetAspectRatio() const override;
	virtual float GetFOVWithRadians() const override;
	virtual float GetNearZ() const override;
	virtual float GetFarZ() const override;

	virtual void SetAspectRatio(float InRatio) override;
	virtual void SetFOVWithDegrees(float InDegrees) override;
	virtual void SetFOVWithRadians(float InRadians) override;
	virtual void SetNearZ(float InValue) override;
	virtual void SetFarZ(float InValue) override;
	void SetCameraTransform(const Transform& InTransform) override;

	void SetIncludedRenderWorldXXX(IRenderWorld* InWorld) override;

private:

	float _NearZ = 0;
	float _FarZ = 0;
	float _FOV = 0;
	float _AspectRatio = 0;

	IRenderWorld* _OwnerRenderWorld = nullptr;
	Transform _CameraTransform;
};
