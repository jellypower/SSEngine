#pragma once
#include "SSEngineDefault/Public/SSVector.h"

struct alignas(16) CBARenderEnvParam
{
    XMMATRIX VPMatrix;
    XMVECTOR SunDirection;
    XMVECTOR SunIntensity;
    XMVECTOR ViewerPos;
};
