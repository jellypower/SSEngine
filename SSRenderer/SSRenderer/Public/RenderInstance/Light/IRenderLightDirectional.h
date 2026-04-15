#pragma once
#include "IRenderLight.h"

class IRenderCamera;
struct RenderLightDirectionalDesc;

class IRenderLightDirectional : public IRenderLight
{
public:
	virtual const RenderLightDirectionalDesc& GetDirectionalLightDesc() const = 0;
	virtual XMVECTOR CalcDirectionalLightDirection() const = 0;
	virtual XMMATRIX CalcShadowMapVPMatrix(const IRenderCamera* CameraToUseShadowMap) const = 0;
};
