#pragma once
#include "SSEngineDefault/Private/PCommon/RawInput/SSRawInputProcessorBase.h"

#include <Windows.h>



class Win32SSRawInputProcessor : public SSRawInputProcessorBase
{
public:
	void ProcessInputEventForWindowsInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};
