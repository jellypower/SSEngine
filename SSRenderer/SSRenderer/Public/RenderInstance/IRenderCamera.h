#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;
class GALRenderTarget;


class IRenderCamera : INoncopyable
{
public:
	virtual IRenderWorld* GetIcludedRenderWorld() const = 0;
	virtual const XMMATRIX& GetVPMatrix() const = 0;
	virtual const Transform& GetCameraTransform() const = 0;

	virtual void SetVPMatrix(const XMMATRIX& InMatrix) = 0;
	virtual void SetCameraTransform(const Transform& InTransform) = 0;


	virtual void SetIncludedRenderWorldXXX(IRenderWorld* InWorld) = 0;
};
