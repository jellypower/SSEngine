#pragma once
#include "SSEngineDefault/Public/SSDirectXMathCustom.h"

constexpr int32 DIRECTIONAL_LIGHT_LIMIT = 8;
constexpr int32 POINT_LIGHT_LIMIT = 32;
constexpr int32 SINGLE_SHADOWMAP_CNT_LIMIT = 4;


struct alignas(16) CBADirectionalLight
{
    XMVECTOR Direction;
    XMVECTOR Color;
};

struct alignas(16) CBAPointLight
{
    XMVECTOR Pos;
    XMVECTOR Color;
    float Radius;
};

struct alignas(16) CBARenderLightParam
{
    int32 DirectionalLightCnt;
    int32 PointLightCnt;
    int32 ShadowMapIdxOnDirectionalLights;
    XMVECTOR AmbientLightIntensity;
    XMMATRIX ShadowMapVPMat;
    CBADirectionalLight DirectionalLights[DIRECTIONAL_LIGHT_LIMIT];
    CBAPointLight PointLights[POINT_LIGHT_LIMIT];
};

static_assert(sizeof(CBARenderLightParam) < 64 * 1024);