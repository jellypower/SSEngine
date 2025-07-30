#pragma once
#include "SSEngineDefault/Public/SSDirectXMathCustom.h"

struct alignas(16) CBARenderEnvParam
{
    XMMATRIX VPMatrix;
    XMVECTOR ViewerPos;
};
