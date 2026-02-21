#pragma once
#include "IThreadManager.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"


class SSThreadUtil
{
public:
	inline static bool IsInMainThread()
	{
		return g_ThreadManager->IsInMainThread();
	}
};

