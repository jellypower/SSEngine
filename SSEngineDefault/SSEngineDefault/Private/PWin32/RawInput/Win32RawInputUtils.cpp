#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/RawInput/RawInputUtils.h"

#include "Win32SSRawInputProcessor.h"

void Win32ProcessInputEvent(IRawInputProcessor* InRawInputProcessor, HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
	Win32SSRawInputProcessor* Win32RawInputProcessor = (Win32SSRawInputProcessor*)InRawInputProcessor;
	Win32RawInputProcessor->ProcessInputEventForWindowsInternal(hWnd, message, wParam, lParam);
}
