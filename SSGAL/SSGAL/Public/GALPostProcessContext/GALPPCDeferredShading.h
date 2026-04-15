#pragma once
#include "GALPostProcessContextBase.h"

class GALRenderTarget;

class GALPPCDeferredShading : public GALPostProcessContextBase
{
public:
	virtual void SetRTNormal(GALRenderTarget* InRenderTarget) = 0;
	virtual void SetRTAlbedo(GALRenderTarget* InRenderTarget) = 0;
	virtual void SetRTWorldPos(GALRenderTarget* InRenderTarget) = 0;
	virtual void SetRTMetallicRoughness(GALRenderTarget* InRenderTarget) = 0;
	virtual void SetRTEmissive(GALRenderTarget* InRenderTarget) = 0;
};
