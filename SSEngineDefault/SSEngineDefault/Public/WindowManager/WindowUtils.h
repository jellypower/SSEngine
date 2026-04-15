#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

class IWindowManager;

#ifdef _WINDOWS

#include <Windows.h>
SSENGINEDEFAULT_MODULE_NATIVE void Win32ProcessWindowEvent(IWindowManager* InWindowManager, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
SSENGINEDEFAULT_MODULE_NATIVE void InitWindowManagerWinHandle(IWindowManager* InWindowManager, HWND hMainWnd, RECT MainWndInitalSize, HINSTANCE hInstance);

#endif //_WINDOWS


