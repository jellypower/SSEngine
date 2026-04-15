#include "Win32Window.h"

#include "WindowUtil_Internal.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"


Win32Window::Win32Window(HWND InHwnd, RECT InitWinRect)
{
	_hWnd = InHwnd;


	_WindowSize.X = InitWinRect.right - InitWinRect.left;
	_WindowSize.Y = InitWinRect.bottom - InitWinRect.top;

	_WindowLT.X = InitWinRect.left;
	_WindowLT.Y = InitWinRect.top;
}

void Win32Window::ResizeWindowXXX(uint32 width, uint32 height)
{
	SS_ASSERT(false); // TODO: Impl
}

void Win32Window::PerFrame_OnEndOfFrame()
{
	
}

Vector2i32 Win32Window::GetWindowCenter() const
{
	int32 CenterX = _WindowLT.X + _WindowSize.X / 2;
	int32 CenterY = _WindowLT.Y + _WindowSize.Y / 2;

	return { CenterX, CenterY };
}

Vector2i32 Win32Window::GetWindowLeftTop() const
{
	return _WindowLT;
}

Vector2i32 Win32Window::GetWindowSize() const
{
	return _WindowSize;
}

bool Win32Window::IsMouseVisible() const
{
	return _bVisibleMouse;
}

bool Win32Window::IsForcingMouseCenter() const
{
	return _bForceMouseCenter;
}

void Win32Window::SetVisibleMouse(bool bVisible)
{
	_bVisibleMouse = bVisible;

	if (g_MainWindowManager->GetFocusingWindow() == this)
	{
		ShowCursorForce_Internal(bVisible);
	}
}

void Win32Window::SetForceMouseCenter(bool bForce)
{
	_bForceMouseCenter = bForce;
}

HWND Win32Window::GetHWND() const
{
	return _hWnd;
}