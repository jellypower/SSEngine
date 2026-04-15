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


public:
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;

	virtual void OnGameObjectTransformCommited() override;

public:
	IRenderCamera* GetRenderCamera() const { return _RenderCamera; }
	float GetNearZ() const;
	float GetFarZ() const;

	void SetAspectRatio(float InRatio);
	void SetFOVWithDegrees(float InDegrees);
	void SetFOVWithRadians(float InRadians);
	void SetNearZ(float InValue); 
	void SetFarZ(float InValue);
};