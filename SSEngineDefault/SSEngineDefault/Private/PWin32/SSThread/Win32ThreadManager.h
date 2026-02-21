#pragma once
#include "SSEngineDefault/Private/PCommon/SSThread/PCommmonThreadManager.h"
#include "SSEngineDefault/Public/RawInput/RawInputUtils.h"


class Win32ThreadManager : public PCommmonThreadManager
{
private:
	HANDLE _hMainThread = NULL;

public:
	bool IsInMainThread() const override;

public:
	void SetMainThreadHandle(HANDLE hMainThread);
};
