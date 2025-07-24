#pragma once
#include "Public/GALRenderInstance/GALRIMetadata.h"

class GALRenderTarget;
enum class ELightType;



class GALRIShadowMapMetadata : public GALRIMetadata
{
public:
	virtual ELightType GetLightType() const = 0;
	virtual GALRenderTarget* GetShadowMap() const = 0;
};
