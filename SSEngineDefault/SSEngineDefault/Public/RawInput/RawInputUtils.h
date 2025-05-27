#pragma once

class SSRawInputProcessorBase;

#ifdef _WINDOWS

#include <Windows.h>
void Win32ProcessInputEvent(SSRawInputProcessorBase* InRawInputProcessor, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif