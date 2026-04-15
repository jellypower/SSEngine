// IncludeShader Constants

#ifndef IS_CONSTANTS_HLSL
#define IS_CONSTANTS_HLSL

const static float4x4 IDENTITY_MATRIX =
{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

const static float4x4 ZERO_MATRIX =
{
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
};

const static int INT_MAX = 2147483647;
const static unsigned int INVALID_IDX = -1;
static const float PI = 3.14159265;

static const float3 kDielectricSpecular = float3(0.04, 0.04, 0.04);
// 유전체(dielectric material)는 전기장 안에서 극성을 지니게 되는 절연체이다.

#endif