#pragma once
#include <Windows.h>


#include "SSNativeTypes.h"
#include "SSDebugLogger.h"


enum class EKeyCode : int32 {
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,

	Unknown,
	Count
};

enum class EMouseCode : int32 {
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_WHEEL_CLICK,
	MOUSE_WHEEL_UP,
	MOUSE_WHEEL_DOWN,

	Unknown,
	Count
};

class SSInput
{
public:
	static SSInput* Get();

	static bool GetKey(EKeyCode keyCode) { return _instance->_keyState[static_cast<int32>(keyCode)]; }
	static bool GetKeyDown(EKeyCode keyCode)
	{
		return !_instance->_prevFrameKeyState[static_cast<int32>(keyCode)] &&
			_instance->_keyState[static_cast<int32>(keyCode)];
	}
	static bool GetKeyUp(EKeyCode keyCode)
	{
		return _instance->_prevFrameKeyState[static_cast<int32>(keyCode)] &&
			!_instance->_keyState[static_cast<int32>(keyCode)];
	}

	static bool GetMouse(EMouseCode mouseCode) { return _instance->_mouseState[static_cast<int32>(mouseCode)]; }
	static bool GetMouseDown(EMouseCode mouseCode)
	{
		return _instance->_prevFrameMouseState[static_cast<int32>(mouseCode)] == false &&
			_instance->_mouseState[static_cast<int32>(mouseCode)];
	}
	static bool GetMouseUp(EMouseCode mouseCode)
	{
		return _instance->_prevFrameMouseState[static_cast<int32>(mouseCode)]&&
			_instance->_mouseState[static_cast<int32>(mouseCode)] == false;
	}

	static Vector2f GetMouseDelta() { return _instance->GetMouseDeltaInternal(); }
	static float GetMouseWheelDelta() { return _instance->GetMouseWheelDeltaInternal(); }


private:
	static SSInput* _instance;
	bool _prevFrameKeyState[static_cast<int32>(EKeyCode::Count)];
	bool _keyState[static_cast<int32>(EKeyCode::Count)];
	bool _prevFrameMouseState[static_cast<int32>(EMouseCode::Count)];
	bool _mouseState[static_cast<int32>(EMouseCode::Count)];

	float _mouseWheelDelta = 0.f;

	Vector2i32 _mousePos;
	Vector2i32 _mouseDelta;


private:
	SSInput();

	FORCEINLINE static void Release();

	void ClearCurInputState();
	void ProcessInputEndOfFrame();
	void ProcessInputEventForWindowsInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	Vector2f GetMouseDeltaInternal() const;
	float GetMouseWheelDeltaInternal() const;
};


FORCEINLINE SSInput* SSInput::Get() {
	if (_instance == nullptr)
		_instance = DBG_NEW SSInput();
	return _instance;
}



FORCEINLINE void SSInput::Release() {
	delete _instance;
	_instance = nullptr;
}