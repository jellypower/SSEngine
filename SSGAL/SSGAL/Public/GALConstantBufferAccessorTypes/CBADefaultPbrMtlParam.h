#pragma once
#include "SSEngineDefault/Public/SSDirectXMathCustom.h"

// Constant Buffer Accessor Default Pbr Material Param
struct alignas(16) CBADefaultPbrMtlParam
{
    XMVECTOR baseColorFactor;
    XMVECTOR emissiveFactor;
    float normalTextureScale;
    float metallicFactor;
    float roughnessFactor;
};
