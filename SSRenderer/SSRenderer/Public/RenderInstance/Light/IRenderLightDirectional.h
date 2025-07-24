#pragma once
#include "IRenderLight.h"

struct RenderLightDirectionalDesc;

class IRenderLightDirectional : public IRenderLight
{
public:
	virtual const RenderLightDirectionalDesc& GetDirectionalLightDesc() const = 0;
	virtual XMVECTOR CalcDirectionalLightDirection() const = 0;
	virtual XMMATRIX CalcShadowMapVPMatrix() const = 0;
};
