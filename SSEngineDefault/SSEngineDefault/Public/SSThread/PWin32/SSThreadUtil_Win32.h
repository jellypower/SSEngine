#pragma once
#include <Windows.h>
#include "SSEngineDefault/ModuleExportKeyword.h"


class IThreadManager;
SSENGINEDEFAULT_MODULE_NATIVE void SetMainThreadHandle(IThreadManager* ThreadManager, HANDLE hMainThread);