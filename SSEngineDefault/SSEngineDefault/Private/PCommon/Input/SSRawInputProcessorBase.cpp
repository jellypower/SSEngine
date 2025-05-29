#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/RawInput/SSRawInputProcessorBase.h"

#include <string.h>

SSRawInputProcessorBase::SSRawInputProcessorBase()
{
	memset(_prevFrameKeyState, 0, sizeof(_prevFrameKeyState));
	memset(_keyState, 0, sizeof(_keyState));
	memset(_prevFrameMouseState, 0, sizeof(_prevFrameMouseState));
	memset(_mouseState, 0, sizeof(_mouseState));
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
