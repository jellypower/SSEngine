#pragma once
#include "SSContentsBase/Public/ContentBase/SComponentBase.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderCamera;



constexpr float CAM_FOV_MIN = 0.01f;
constexpr float CAM_FOV_MAX = XM_PI * 0.99f;

class SSCONTENTBASE_MODULE SCameraComponent : public SComponentBase
{
private:
	IRenderCamera* _RenderCamera = nullptr;

	XMMATRIX _ViewMat;
	XMMATRIX _ProjMat;

	float _FOV = 0.f; // FOV's unit is "Radian", "Y Degree" 
	float _NearZ = 0.f;
	float _FarZ = 0.f;

public:
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;

	virtual void OnGameObjectTransformCommited() override;

public:
	IRenderCamera* GetRenderCamera() const { return _RenderCamera; }

	void SetFOVWithDegrees(float InDegrees);
	void SetFOVWithRadians(float InRadians);
	void SetNearZ(float InValue); 
	void SetFarZ(float InValue);

	void CommitCameraRenderInfo();

};