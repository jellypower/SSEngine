#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "KeyCodeEnums.h"


class IRawInputProcessor : public INoncopyable
{
protected:
	bool _prevFrameKeyState[static_cast<int32>(EKeyCode::Count)];
	bool _keyState[static_cast<int32>(EKeyCode::Count)];
	bool _prevFrameMouseState[static_cast<int32>(EMouseCode::Count)];
	bool _mouseState[static_cast<int32>(EMouseCode::Count)];

	float _mouseWheelDelta = 0.f;

	Vector2i32 _mousePos;
	Vector2i32 _mouseDelta;

public:
	virtual Vector2f GetMouseDelta() const = 0;

public:
	virtual void ResetCurInputState() = 0;
	virtual void ProcessInputStartOfFrame() = 0;
	virtual void ProcessInputEndOfFrame() = 0;




public:
	inline bool GetKey(EKeyCode keyCode) const { return _keyState[static_cast<int32>(keyCode)]; }
	inline bool GetKeyDown(EKeyCode keyCode) const { return _prevFrameKeyState[static_cast<int32>(keyCode)] == false && _keyState[static_cast<int32>(keyCode)]; }
	inline bool GetKeyUp(EKeyCode keyCode) const { return _prevFrameKeyState[static_cast<int32>(keyCode)] && _keyState[static_cast<int32>(keyCode)] == false; }

	inline bool GetMouse(EMouseCode mouseCode) const { return _mouseState[static_cast<int32>(mouseCode)]; }
	inline bool GetMouseDown(EMouseCode mouseCode) const { return _prevFrameMouseState[static_cast<int32>(mouseCode)] == false && _mouseState[static_cast<int32>(mouseCode)]; }
	inline bool GetMouseUp(EMouseCode mouseCode) const { return _prevFrameMouseState[static_cast<int32>(mouseCode)] && _mouseState[static_cast<int32>(mouseCode)] == false; }

	inline float GetMouseWheelDelta() const { return _mouseWheelDelta; }

	inline Vector2i32 GetMousePos() const { return _mousePos; }
};