#pragma once
#include "SSVector.h"

using namespace DirectX;

namespace SS {

	FORCEINLINE float RadToDegrees(float InRad)
	{
		return InRad / XM_PI * 180.f;
	}

	FORCEINLINE float DegToRadians(float InDeg)
	{
		return InDeg / 180.f * XM_PI;
	}

	FORCEINLINE float SqrDistance(const Vector4f& lhs, const Vector4f& rhs)
	{
		return (rhs - lhs).Get3DSqrLength();
	}

	FORCEINLINE float SqrDistance(const Vector2f& lhs, const Vector2f& rhs)
	{
		return (rhs - lhs).GetSqrLength();
	}

	FORCEINLINE float Dot3D(const Vector4f& lhs, const Vector4f& rhs)
	{
		XMVECTOR mul = lhs.SimdVec * rhs.SimdVec;
		return mul.m128_f32[0] + mul.m128_f32[1] + mul.m128_f32[2];
	}


	FORCEINLINE float abs(float InValue)
	{
		return InValue < 0 ? -InValue : InValue;
	}


	FORCEINLINE Quaternion Slerp(Quaternion lhs, Quaternion rhs, float alpha)
	{
		return Quaternion(XMQuaternionSlerp(lhs.SimdVec, rhs.SimdVec, alpha));
	}

	FORCEINLINE Vector4f Lerp(Vector4f lhs, Vector4f rhs, float alpha)
	{
		return (rhs.SimdVec - lhs.SimdVec) * alpha + rhs.SimdVec;
	}

	FORCEINLINE Transform Lerp(Transform lhs, Transform rhs, float alpha)
	{
		return Transform(
			Lerp(lhs.Position, rhs.Position, alpha),
			Slerp(lhs.Rotation, rhs.Rotation, alpha),
			Lerp(lhs.Scale, rhs.Scale, alpha));
	}

};