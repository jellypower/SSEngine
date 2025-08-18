#pragma once
#include "SSEngineDefault/Public/SSDirectXMathCustom.h"


struct alignas(16) SBASkinningJointMatrix 
{
	XMMATRIX WMatrix;
	XMMATRIX RotMatrix;
};