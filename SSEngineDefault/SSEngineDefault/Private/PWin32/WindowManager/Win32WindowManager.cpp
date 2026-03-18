#include "Win32WindowManager.h"

#include "Win32Window.h"
#include "WindowUtil_Internal.h"

Win32WindowManager::~Win32WindowManager()
{
	delete _MainWindow;
}

IWindow* Win32WindowManager::GetMainWindow() const
{
	return _MainWindow;
}

IWindow* Win32WindowManager::GetFocusingWindow() const
{
	return _FocusingWindow;
}

void Win32WindowManager::ProcessWindowEndOfFrame()
{

}

void Win32WindowManager::Win32ProcessWindowEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			SetFocusWindow(NULL);
		}
		else if (LOWORD(wParam) == WA_ACTIVE)
		{
			SetFocusWindow(hWnd);
		}
		break;

	default:
		SS_ASSERT(false);
		break;
	}
}

void Win32WindowManager::InitWindowManagerWinHandle(IWindowManager* InWindowManager, HWND hMainWnd, RECT MainWndInitalSize, HINSTANCE hInstance)
{
	_hMainWnd = hMainWnd;
	_hInstance = hInstance;

	_MainWindow = DBG_NEW Win32Window(hMainWnd, MainWndInitalSize);
	_Windows.PushBack(_MainWindow);

	SetFocusWindow(_hMainWnd);
}

IWindow* Win32WindowManager::FindWindowOfHWND(HWND InHWnd) const
{
	for (IWindow* WindowItem: _Windows)
	{
		if (static_cast<Win32Window*>(WindowItem)->GetHWND() == InHWnd)
		{
			return WindowItem;
		}
	}

	return nullptr;
}

void Win32WindowManager::SetFocusWindow(HWND hNewWindow)
{
	IWindow* Window = FindWindowOfHWND(hNewWindow);
	_FocusingWindow = Window;

	if (_FocusingWindow != nullptr)
	{
		bool bVisible = _FocusingWindow->IsMouseVisible();
		ShowCursorForce_Internal(bVisible);
	}
	else
	{
		ShowCursorForce_Internal(true);
	}
}