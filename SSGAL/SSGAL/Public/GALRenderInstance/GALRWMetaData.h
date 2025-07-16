#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IRenderWorld;

class GALRWMetaData : public INoncopyable
{
protected:
	IRenderWorld* _OwnerRenderWorld = nullptr;
};
