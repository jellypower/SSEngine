#pragma once
#include "SSEngineDefault/Public/ISSUnknown.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IThreadManager : ISSUnknown
{
public:
	virtual bool IsInMainThread() const = 0;
};
