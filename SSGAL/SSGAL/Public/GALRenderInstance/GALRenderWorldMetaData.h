#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;

class GALRenderWorldMetaData : public INoncopyable
{
protected:
	IRenderWorld* _OwnerRenderWorld = nullptr;
};
