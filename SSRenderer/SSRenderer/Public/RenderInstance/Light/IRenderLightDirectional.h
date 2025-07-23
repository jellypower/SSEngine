#pragma once
#include "IRenderLight.h"

struct RenderLightDirectionalDesc
{
	ELightType Type;
	bool bEnableShadowMap;
	Vector2f ShadowMapSize;
};

class IRenderLightDirectional : public IRenderLight
{
public:
	virtual XMMATRIX CalcShadowMapVPMatrix() const = 0;

	virtual void InjectShadowMapXXX(GALRenderTarget* ShadowMapToHandover) = 0;
	virtual GALRenderTarget* GetShadowMap() const = 0; // Cascade 지원되게 만들기
	virtual void ReleaseShadowMap() = 0;
};
