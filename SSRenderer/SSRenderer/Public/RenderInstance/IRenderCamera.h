#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;
class GALRenderTarget;


class IRenderCamera : INoncopyable
{
public:
	virtual IRenderWorld* GetIcludedRenderWorld() const = 0;
	virtual XMMATRIX CalcVPMatrix() const = 0;
	virtual const Transform& GetCameraTransform() const = 0;

	virtual float GetAspectRatio() const = 0;
	virtual float GetFOVWithRadians() const = 0;
	virtual float GetNearZ() const = 0;
	virtual float GetFarZ() const = 0;

	virtual void SetAspectRatio(float InRatio) = 0;
	virtual void SetFOVWithDegrees(float InDegrees) = 0;
	virtual void SetFOVWithRadians(float InRadians) = 0;
	virtual void SetNearZ(float InValue) = 0;
	virtual void SetFarZ(float InValue) = 0;
	virtual void SetCameraTransform(const Transform& InTransform) = 0;


	virtual void SetIncludedRenderWorldXXX(IRenderWorld* InWorld) = 0;
};
