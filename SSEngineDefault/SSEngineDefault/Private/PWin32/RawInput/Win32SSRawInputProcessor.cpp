#include "Win32SSRawInputProcessor.h"

#include <windowsx.h>

#include "Win32RawInputProcessUtil.h"
#include "SSEngineDefault/Public/WindowManager/IWindow.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"
#include "SSEngineDefault/Public/WindowManager/SSWindowInfo.h"

void Win32SSRawInputProcessor::ProcessInputEventForWindowsInternal(HWND hWnd, UINT message, WPARAM wParam,
                                                                   LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		_keyState[static_cast<int32>(WindowsVKCodeToSSKeyCode(wParam))] = true;
	}
	break;

	case WM_KEYUP:
	{
		_keyState[static_cast<int32>(WindowsVKCodeToSSKeyCode(wParam))] = false;
	}
	break;

	case WM_LBUTTONDOWN:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_LEFT)] = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_LEFT)] = false;
	}
	break;

	case WM_RBUTTONDOWN:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_RIGHT)] = true;
	}
	break;

	case WM_RBUTTONUP:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_RIGHT)] = false;
	}
	break;

	case WM_MBUTTONDOWN:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_WHEEL_CLICK)] = true;
	}
	break;

	case WM_MBUTTONUP:
	{
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_WHEEL_CLICK)] = false;
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int16 Delta = GET_WHEEL_DELTA_WPARAM(wParam);
		_mouseState[static_cast<int32>(EMouseCode::MOUSE_WHEEL_CLICK)] = false;
		_mouseWheelDelta = Delta;
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (SSMainWindowInfo::IsFocusingWindowForcingCenter() == false)
		{
			Vector2i32 newPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			_mouseDelta = newPos - _mousePos;
			_mousePos = newPos;
		}
	}
	break;

	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			ResetCurInputState();
		}
	}
	case WM_MOUSELEAVE:
	{
		ResetCurInputState();
	}
	break;

	default:
		SS_ASSERT(false);
	}
}

void Win32SSRawInputProcessor::ProcessInputStartOfFrame()
{
	if (SSMainWindowInfo::IsFocusingWindowForcingCenter())
	{
		IWindow* Focusing = g_MainWindowManager->GetFocusingWindow();
		Vector2i32 Center = Focusing->GetWindowCenter();

		POINT cur;
		::GetCursorPos(&cur);
		Vector2i32 newPos(cur.x, cur.y);

		_mouseDelta = newPos - Center;
		_mousePos = Center;

		::SetCursorPos(Center.X, Center.Y);
	}
}
