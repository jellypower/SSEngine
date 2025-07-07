#pragma once
#include "SSContentsBase/Public/SComponentBase.h"

class IRenderCamera;
class GALRenderTarget;



constexpr float CAM_FOV_MIN = 0.01f;
constexpr float CAM_FOV_MAX = XM_PI * 0.99f;

class SCameraComponent : public SComponentBase
{
public:
	GALRenderTarget* _RenderTarget = nullptr;

private:
	IRenderCamera* _RenderCamera = nullptr;

	float _FOV; // FOV's unit is "Radian", "Y Degree" 
	float _NearZ;
	float _FarZ;

public:
	virtual void PostConstructHierarchy() override;
	virtual void PreDestructHierarchy() override;

public:
	IRenderCamera* GetRenderCamera() const { return _RenderCamera; }
	XMMATRIX GetVPMatrix() const;

	void SetFOVWithRadians(float InRadians);
	void SetFOVWithDegrees(float InDegrees);
	void SetNearZ(float InValue) { _NearZ = InValue; }
	void SetFarZ(float InValue) { _FarZ = InValue; }

protected:
	void ConstructRenderTarget();
	void DestructRenderTarget();
};
