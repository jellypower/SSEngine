#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

class IRawInputProcessor;

#ifdef _WINDOWS

#include <Windows.h>
SSENGINEDEFAULT_MODULE_NATIVE void Win32ProcessInputEvent(IRawInputProcessor* InRawInputProcessor, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif //_WINDOWS