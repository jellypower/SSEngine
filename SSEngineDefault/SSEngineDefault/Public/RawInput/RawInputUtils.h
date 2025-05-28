#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

class SSRawInputProcessorBase;

#ifdef _WINDOWS

#include <Windows.h>
SSENGINEDEFAULT_MODULE_NATIVE void Win32ProcessInputEvent(SSRawInputProcessorBase* InRawInputProcessor, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif //_WINDOWS