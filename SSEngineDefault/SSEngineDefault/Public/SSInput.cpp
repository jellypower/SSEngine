#include <windowsx.h>

#include "SSInput.h"
#include "SSFrameInfo.h"

SSInput* SSInput::_instance = nullptr;

SSInput::SSInput()
{
	memset(_prevFrameKeyState, 0, sizeof(_prevFrameKeyState));
	memset(_keyState, 0, sizeof(_keyState));
	memset(_prevFrameMouseState, 0, sizeof(_prevFrameMouseState));
	memset(_mouseState, 0, sizeof(_mouseState));
}

EKeyCode WindowsVKCodeToSSKeyCode(WPARAM wParam)
{
	switch (wParam)
	{
	case 'A': return EKeyCode::KEY_A;
	case 'C': return EKeyCode::KEY_C;
	case 'D': return EKeyCode::KEY_D;
	case 'E': return EKeyCode::KEY_E;
	case 'Q': return EKeyCode::KEY_Q;
	case 'S': return EKeyCode::KEY_S;
	case 'W': return EKeyCode::KEY_W;
	case 'Z': return EKeyCode::KEY_Z;
	case 'M': return EKeyCode::KEY_M;
	case 'N': return EKeyCode::KEY_N;

	case VK_UP: return EKeyCode::KEY_UP;
	case VK_DOWN: return EKeyCode::KEY_DOWN;
	case VK_LEFT: return EKeyCode::KEY_LEFT;
	case VK_RIGHT: return EKeyCode::KEY_RIGHT;
	}

	return EKeyCode::Unknown;
}

void SSInput::ProcessInputEventForWindowsInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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


 void SSInput::ClearCurInputState()
 {
	 SS_LOG("Clear input event!!\n");
	 memset(_keyState, 0, sizeof(_keyState));
	 memset(_mouseState, 0, sizeof(_mouseState));
 }

void SSInput::ProcessInputEndOfFrame()
{
	_mouseWheelDelta = 0.f;
	_mouseDelta = Vector2i32::Zero;
	memcpy(_prevFrameKeyState, _keyState, sizeof(_keyState));
	memcpy(_prevFrameMouseState, _mouseState, sizeof(_prevFrameMouseState));
}


Vector2f SSInput::GetMouseDeltaInternal() const
{
	Vector2ui32 winSize = SSFrameInfo::GetWindowSize();

	return Vector2f(
		(float)_mouseDelta.X / winSize.X,
		-(float)_mouseDelta.Y / winSize.Y
	);
}

float SSInput::GetMouseWheelDeltaInternal() const
{
	return _mouseWheelDelta;
}
