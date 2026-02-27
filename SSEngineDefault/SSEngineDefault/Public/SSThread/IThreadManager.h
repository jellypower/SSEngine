#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IThreadManager : INoncopyable
{
public:
	virtual bool IsInMainThread() const = 0;
};
