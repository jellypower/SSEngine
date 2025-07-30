#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;

class GALRWMetaData : public INoncopyable
{
public:
	virtual IRenderWorld* GetOwnerRenderWorld() const = 0;
};
