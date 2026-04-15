#include "SSEngineDefault/Public/WindowManager/WindowUtils.h"

#include "Win32WindowManager.h"

void Win32ProcessWindowEvent(IWindowManager* InWindowManager, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32WindowManager* lWin32WindowManager = (Win32WindowManager*)InWindowManager;
	lWin32WindowManager->Win32ProcessWindowEvent(hWnd, message, wParam, lParam);
}

void InitWindowManagerWinHandle(IWindowManager* InWindowManager, HWND hMainWnd, RECT MainWndInitalSize, HINSTANCE hInstance)
{
	Win32WindowManager* lWin32WindowManager = (Win32WindowManager*)InWindowManager;
	lWin32WindowManager->InitWindowManagerWinHandle(InWindowManager, hMainWnd, MainWndInitalSize, hInstance);
}
