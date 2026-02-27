#include "Win32ThreadManager.h"

bool Win32ThreadManager::IsInMainThread() const
{
	DWORD CurThreadID = ::GetCurrentThreadId();
	DWORD MainThreadID = GetThreadId(_hMainThread);

	return CurThreadID == MainThreadID;
}

void Win32ThreadManager::SetMainThreadHandle(HANDLE hMainThread)
{
	_hMainThread = hMainThread;
}
