#pragma once
#include "SSNativeKeywords.h"
#include <DirectXMath.h>

using namespace DirectX;

FORCEINLINE XMMATRIX InverseRigid(XMMATRIX InMatrix)
{
	// transpose
	XMMATRIX InverseRigid;
	InverseRigid.r[0].m128_f32[0] = InMatrix.r[0].m128_f32[0];
	InverseRigid.r[0].m128_f32[1] = InMatrix.r[1].m128_f32[0];
	InverseRigid.r[0].m128_f32[2] = InMatrix.r[2].m128_f32[0];
	InverseRigid.r[0].m128_f32[3] = 0;
	InverseRigid.r[1].m128_f32[0] = InMatrix.r[0].m128_f32[1];
	InverseRigid.r[1].m128_f32[1] = InMatrix.r[1].m128_f32[1];
	InverseRigid.r[1].m128_f32[2] = InMatrix.r[2].m128_f32[1];
	InverseRigid.r[1].m128_f32[3] = 0;
	InverseRigid.r[2].m128_f32[0] = InMatrix.r[0].m128_f32[2];
	InverseRigid.r[2].m128_f32[1] = InMatrix.r[1].m128_f32[2];
	InverseRigid.r[2].m128_f32[2] = InMatrix.r[2].m128_f32[2];
	InverseRigid.r[2].m128_f32[3] = 0;
	InverseRigid.r[3] = { 0,0,0,1 };

	// move translation backward
	XMVECTOR translation = InMatrix.r[3];
	translation = XMVector4Transform(translation, InverseRigid);
	InverseRigid.r[3].m128_f32[0] = -translation.m128_f32[0];
	InverseRigid.r[3].m128_f32[1] = -translation.m128_f32[1];
	InverseRigid.r[3].m128_f32[2] = -translation.m128_f32[2];
	InverseRigid.r[3].m128_f32[3] = 1;

	return InverseRigid;
}