#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SObject/Public/SObjHashCode.h"


class GALRenderTarget;

enum class ELightType
{
	None = -1,

	Directional,

};

class IRenderLight : public INoncopyable
{
public:
	virtual ELightType GetLightType() = 0;
	virtual SObjHashCode GetGameObjectID() const = 0;

	virtual GALRenderTarget* GetShadowMap() const = 0;
	virtual XMMATRIX GetVPMatrix() const = 0;
	virtual const Transform& GetLightTransform() const = 0;
};