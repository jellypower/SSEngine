#pragma once
#include <DirectXMath.h>
#include <math.h>
#include <immintrin.h>

#include "SSNativeKeywords.h"

#include "SSDebugLogger.h"

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


// DirecXMath의 XMQuaternionRotationRollPitchYawFromVector함수는 Z, X, Y 순서로 작동합니다.
FORCEINLINE XMVECTOR XMEulerFromQuaternion(const XMVECTOR& q)
{
	float x = q.m128_f32[0];
	float y = q.m128_f32[1];
	float z = q.m128_f32[2];
	float w = q.m128_f32[3];

	float ysqr = y * y;

	float t0 = +2.0f * (w * x - y * z);
	t0 = t0 < -1 ? -1 : t0;
	t0 = t0 > 1 ? 1 : t0;
	float X = asin(t0);


	float Y;
	float Z;

	if (-0.999f < t0 && t0 < 0.999f)
	{
		float t1 = +2.0f * (w * y + z * x);
		float t2 = 1.0f - 2.0f * (x * x + ysqr);
		Y = atan2(t1, t2);

		float t3 = +2.0f * (w * z + x * y);
		float t4 = 1.0f - 2.0f * (x * x + z * z);
		Z = atan2(t3, t4);
	}
	else
	{
		// t0값이 -1이나 1에 가깝다는건 euler좌표계 기준으로 gimbal-lock이 생긴다는 뜻이다.
		// 그래서 해당 상황에 대해선 euler변환에 대해 별도의 계산식이 필요하고 해당 상황을
		// Singularity라고 한다.
		// 이에 대해 수학적으로 참고한 문서는 다음과 같다
		//	- https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		//  - https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
		// 위 문서중에서 euclideanspace의 코드를 참조했다.
		// heading = 2 * atan2(q1.x,q1.w); 코드에서 q1.x를 y로 바꿨을 뿐이다.
		// (왜냐면 XMQuaternionRotationRollPitchYawFromVector 함수의 Quaternion변환 순서는 ZXY이기 때문이다.
		// 위 식은 ZYX순서로 적용하는 공식이기 때문에 Y값을 singularity로 쓰지만 DirectXMath는 X값을 Singularity로 쓰기 때문에
		// X값과 Y값이 바뀌어야 한다.

		Z = 0.0f;
		Y = 2 * atan2(y, w);
	}




	XMVECTOR angles;
	angles.m128_f32[0] = X;
	angles.m128_f32[1] = Y;
	angles.m128_f32[2] = Z;
	angles.m128_f32[3] = 0.f;

	return angles;
}

FORCEINLINE bool XMAlmostEqual(const XMVECTOR& v1, const XMVECTOR& v2, float threshold = 0.001)
{
	SS_ASSERT(threshold > 0);
	const XMVECTOR MinThreshold = { -threshold, -threshold , -threshold , -threshold };
	const XMVECTOR MaxThreshold = { threshold, threshold , threshold , threshold };

	XMVECTOR Diff = v2 - v1;

	__m128 Lesser = _mm_cmp_ps(Diff, MaxThreshold, _CMP_LT_OS);
	__m128 Greater = _mm_cmp_ps(Diff, MinThreshold, _CMP_GT_OS);

	__m128 BitWise = _mm_and_ps(Lesser, Greater);

	bool bResult = 
		BitWise.m128_u32[0] &
		BitWise.m128_u32[1] & 
		BitWise.m128_u32[2] &
		BitWise.m128_u32[3];
	return bResult;
}