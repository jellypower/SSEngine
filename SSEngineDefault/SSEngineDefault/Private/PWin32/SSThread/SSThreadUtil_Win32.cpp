#include "SSEngineDefault/Public/SSThread/PWin32/SSThreadUtil_Win32.h"

#include "Win32ThreadManager.h"

void SetMainThreadHandle(IThreadManager* ThreadManager, HANDLE hMainThread)
{
	Win32ThreadManager* Win32TM = static_cast<Win32ThreadManager*>(ThreadManager);
	Win32TM->SetMainThreadHandle(hMainThread);
}
