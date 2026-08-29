#include "SSRawInputProcessorBase.h"

#include <string.h>

#include "SSEngineDefault/Public/WindowManager/IWindow.h"
#include "SSEngineDefault/Public/WindowManager/SSWindowInfo.h"

SSRawInputProcessorBase::SSRawInputProcessorBase()
{
	memset(_prevFrameKeyState, 0, sizeof(_prevFrameKeyState));
	memset(_keyState, 0, sizeof(_keyState));
	memset(_prevFrameMouseState, 0, sizeof(_prevFrameMouseState));
	memset(_mouseState, 0, sizeof(_mouseState));
}

void SSRawInputProcessorBase::Release()
{
	delete this;
}

Vector2f SSRawInputProcessorBase::GetMouseDelta() const
{
	IWindow* FocusingWin = g_MainWindowManager->GetFocusingWindow();
	if (FocusingWin == nullptr)
	{
		return Vector2f::Zero;
	}

	Vector2i32 winSize = FocusingWin->GetWindowSize();
	return Vector2f((float)_mouseDelta.X / winSize.X, -(float)_mouseDelta.Y / winSize.Y);
}


void SSRawInputProcessorBase::ResetCurInputState()
 {
	 memset(_keyState, 0, sizeof(_keyState));
	 memset(_mouseState, 0, sizeof(_mouseState));
 }

void SSRawInputProcessorBase::ProcessInputEndOfFrame()
{
	_mouseWheelDelta = 0.f;
	_mouseDelta = Vector2i32::Zero;
	memcpy(_prevFrameKeyState, _keyState, sizeof(_keyState));
	memcpy(_prevFrameMouseState, _mouseState, sizeof(_prevFrameMouseState));
}
