#pragma once
#include "SObject/Public/SComponentBase.h"

class GALRenderTarget;



constexpr float CAM_FOV_MIN = 0.01f;
constexpr float CAM_FOV_MAX = XM_PI * 0.99f;

class SCameraComponent : public SComponentBase
{
public:
	GALRenderTarget* _RenderTarget = nullptr;

public:
	virtual void PostConstructHierarchy() override;
	virtual void PreDestructHierarchy() override;

public:
	XMMATRIX GetVPMatrix() const;


	void SetFOVWithRadians(float InRadians);
	void SetFOVWithDegrees(float InDegrees);
	void SetNearZ(float InValue) { _NearZ = InValue; }
	void SetFarZ(float InValue) { _FarZ = InValue; }

protected:
	void ConstructRenderTarget();
	void DestructRenderTarget();

private:
	float _FOV; // FOV's unit is "Radian", "Y Degree" 
	float _NearZ;
	float _FarZ;

};
