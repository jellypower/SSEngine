#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;
class GALRenderTarget;


class IRenderCamera : INoncopyable
{
public:
	virtual IRenderWorld* GetIcludedRenderWorld() const = 0;
	virtual GALRenderTarget* GetSpecificRenderTarget() const = 0;
	virtual const XMMATRIX& GetVPMatrix() const = 0;
	virtual const Transform& GetCameraTransform() const = 0;
};
