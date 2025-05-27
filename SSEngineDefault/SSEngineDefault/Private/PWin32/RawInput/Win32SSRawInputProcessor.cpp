#include "Win32SSRawInputProcessor.h"

#include <windowsx.h>

#include "Win32RawInputProcessUtil.h"

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
		Vector2i32 newPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		_mouseDelta = newPos - _mousePos;

		_mousePos = newPos;
	}
	break;

	default:
		assert(false);
	}
}
