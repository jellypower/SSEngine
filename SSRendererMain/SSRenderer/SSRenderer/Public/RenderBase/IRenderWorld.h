#pragma once
#include "SObject/Public/SObjHashCode.h"

class IRenderInstance;

class IRenderWorld : public INoncopyable
{
public:
	virtual bool IsAnyInstanceRemainInWorld() const = 0;

	virtual void AddToWorld(IRenderInstance* InRenderInstance) = 0;
	virtual void RemoveFromWorld(SObjHashCode RenderInstanceIDToRemove) = 0;
};
