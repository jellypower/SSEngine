#pragma once
#include "SRenderComponentBase.h"
#include "SRenderLightComponent.h"

#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"


class IRenderLightDirectional;

class SSCONTENTBASE_MODULE SRenderLightDirectionalComponent : public SRenderLightComponent
{
public:
	RenderLightDirectionalDesc _Desc;

public:
	virtual void PostConstructHierarchy() override;
	virtual void OnEnterTheWorld() override;
	virtual void OnExitTheWorld() override;
	virtual void PreDestructHierarchy() override;

	virtual void OnGameObjectTransformCommited() override;
};
