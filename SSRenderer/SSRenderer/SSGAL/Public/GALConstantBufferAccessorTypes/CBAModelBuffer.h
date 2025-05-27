#pragma once
#include "SSEngineDefault/Public/SSDirectXMathCustom.h"

// Constant Buffer Accessor Type
struct alignas(16) CBAModelBuffer
{
    XMMATRIX WMatrix;
    XMMATRIX RotMatrix;
};