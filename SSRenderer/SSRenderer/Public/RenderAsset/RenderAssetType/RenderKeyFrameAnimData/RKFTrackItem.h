#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


enum class EInterpMethod : int32
{
	None = 0,

	Linear = 1,
	Cubic = 2
};

struct alignas(16) RKFTrackItem
{
	float _TimeRatio = 0; // [0 ~ 1]
	EInterpMethod _Method = EInterpMethod::Linear;
	int64 _Padding = 0;
	Transform _Transform;
};