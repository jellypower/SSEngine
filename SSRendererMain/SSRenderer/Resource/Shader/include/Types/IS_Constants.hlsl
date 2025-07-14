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


#endif