#pragma once
#include "SSRenderer/Public/RenderInstance/IRenderInstance.h"

class GALRenderDeviceContext;
class GALRenderTarget;

enum class ELightType
{
	None = -1,

	Directional,
};



class IRenderLight : public IRenderInstance
{
public:
	virtual ELightType GetLightType() const = 0;
	virtual bool IsShadowMapEnabled() const = 0;

	virtual XMVECTOR GetLightColor() const = 0;
	virtual void SetLightIntensity(const XMVECTOR& InLightIntensity) = 0;
};

// TODO: Unity에서 GameObject에 Static 체크하는 것처럼 static체크가 돼있으면 Light에 계속 Draw하지 않게하는 기능 만들기