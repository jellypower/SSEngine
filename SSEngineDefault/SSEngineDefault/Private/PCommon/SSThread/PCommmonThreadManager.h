#pragma once
#include "SSEngineDefault/Public/SSThread/IThreadManager.h"

class PCommmonThreadManager : public IThreadManager
{
public:
	virtual void Release() override;
};
