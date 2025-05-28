#pragma once
#include "KeyCodeEnums.h"
#include "SSRawInputProcessorBase.h"
#include "SSEngineDefault/Public/SSVector.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSInput
{
public:
	static bool GetKey(EKeyCode keyCode) { return g_RawInputProcessor->GetKey(keyCode); }
	static bool GetKeyDown(EKeyCode keyCode) { return g_RawInputProcessor->GetKeyDown(keyCode); }
	static bool GetKeyUp(EKeyCode keyCode) { return g_RawInputProcessor->GetKeyUp(keyCode); }

	static bool GetMouse(EMouseCode mouseCode) { return g_RawInputProcessor->GetMouse(mouseCode); }
	static bool GetMouseDown(EMouseCode mouseCode) { return g_RawInputProcessor->GetMouseDown(mouseCode); }
	static bool GetMouseUp(EMouseCode mouseCode) { return g_RawInputProcessor->GetMouseUp(mouseCode); }

	static float GetMouseWheelDelta() { return g_RawInputProcessor->GetMouseWheelDelta(); }
	static Vector2f GetMouseDelta() { return g_RawInputProcessor->GetMouseDelta(); }
};
