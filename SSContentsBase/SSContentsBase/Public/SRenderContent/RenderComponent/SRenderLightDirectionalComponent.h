#pragma once
#include "SRenderComponentBase.h"

#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"


class IRenderLightDirectional;

class SSCONTENTBASE_MODULE SRenderLightDirectionalComponent : public SComponentBase
{
public:
	RenderLightDirectionalDesc _Desc;

protected:
	IRenderLightDirectional* _DirectionalLight = nullptr;

public:
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;

	virtual void OnGameObjectTransformCommited() override;
};
