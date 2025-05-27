#pragma once
#include "SSEngineDefault/Public/SSVector.h"

struct BoundBox4f
{
	Vector4f Min;
	Vector4f Max;
};

struct BoundBox2f
{
	Vector2f Min;
	Vector2f Max;
};

struct ViewportBox
{
	Vector2f LeftTop;
	Vector2f WidthHeight;
	float MinDepth = 0.f;
	float MaxDepth = 0.f;
};