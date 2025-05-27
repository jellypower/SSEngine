#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"


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