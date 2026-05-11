#pragma once
#include <DirectXMath.h>
#include <math.h>
#include <immintrin.h>

#include "SSNativeKeywords.h"

#include "SSDebugLogger.h"

using namespace DirectX;

FORCEINLINE XMMATRIX InverseRigid(XMMATRIX InMatrix)
{
	XMMATRIX InverseRigid = XMMatrixTranspose(InMatrix);


//	========= XMVectorSetW 에 대한 설명 ========
//  x랑 w 위치를 바꿉니다
//	XMVECTOR vResult = XM_PERMUTE_PS(V, _MM_SHUFFLE(0, 2, 1, 3));
//	넣어준 w값으로 벡터를 만듭니다.
//	XMVECTOR vTemp = _mm_set_ss(w);
//	함수 _mm_move_ss 는 x위치만 vTemp의 x를 넣어주고 yzw는 vResult를 넣어줍니다.
//	vResult = _mm_move_ss(vResult, vTemp);
//	다시 w랑 x위치를 바꿉니다
//	vResult = XM_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 2, 1, 3));
//  이렇게 하면 메모리 오퍼레이션 없이 레지스터 안에서만 W값을 지정해줄 수 있습니다.
//	return vResult;
	InverseRigid.r[0] = XMVectorSetW(InverseRigid.r[0], 0.0f);
	InverseRigid.r[1] = XMVectorSetW(InverseRigid.r[1], 0.0f);
	InverseRigid.r[2] = XMVectorSetW(InverseRigid.r[2], 0.0f);
	InverseRigid.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR T = InMatrix.r[3];
	XMVECTOR invT = XMVector3TransformNormal(T, InverseRigid);
	InverseRigid.r[3] = XMVectorScale(invT, -1.0f);
	InverseRigid.r[3] = XMVectorSetW(InverseRigid.r[3], 1.0f);

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

// Return QuaternionRotation
// 원본 메시가 Z방향을 바라보는 경우에 해당 함수에 Dir을 넣으면 메시가 그 방향으로 회전한다.
FORCEINLINE XMVECTOR XMPitchYawRotFromDir(const XMVECTOR& InDir)
{
	float Yaw = atan2(InDir.m128_f32[0], InDir.m128_f32[2]);
	float FloorLen = sqrt(InDir.m128_f32[2] * InDir.m128_f32[2] + InDir.m128_f32[0] * InDir.m128_f32[0]);
	float Pitch = -atan2(InDir.m128_f32[1], FloorLen);


	// <Pitch, Yaw, Roll, 0>
	XMVECTOR Euler = { Pitch, Yaw, 0, 0 };
	return XMQuaternionRotationRollPitchYawFromVector(Euler);
}


FORCEINLINE XMVECTOR ExtractScale(const XMMATRIX& InMat)
{
	float scaleXSqr = XMVectorGetX(XMVector3LengthSq(InMat.r[0])); // X-axis basis
	float scaleYSqr = XMVectorGetX(XMVector3LengthSq(InMat.r[1])); // Y-axis basis
	float scaleZSqr = XMVectorGetX(XMVector3LengthSq(InMat.r[2])); // Z-axis basis

	return XMVectorSqrt({ scaleXSqr, scaleYSqr, scaleZSqr });
}