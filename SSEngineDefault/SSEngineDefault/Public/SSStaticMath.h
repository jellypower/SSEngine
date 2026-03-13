#pragma once
#include "SSVector.h"

using namespace DirectX;

namespace SS {

	FORCEINLINE constexpr float RadToDegrees(float InRad)
	{
		return InRad / XM_PI * 180.f;
	}

	FORCEINLINE constexpr float DegToRadians(float InDeg)
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


	FORCEINLINE Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float alpha)
	{
		return Quaternion(XMQuaternionSlerp(lhs.SimdVec, rhs.SimdVec, alpha));
	}

	FORCEINLINE Vector4f Lerp(const Vector4f& lhs, const Vector4f& rhs, float alpha)
	{
		return (rhs.SimdVec - lhs.SimdVec) * alpha + lhs.SimdVec;
	}

	FORCEINLINE Transform Lerp(const Transform& lhs, const Transform& rhs, float alpha)
	{
		return Transform(
			Lerp(lhs.Position, rhs.Position, alpha),
			Slerp(lhs.Rotation, rhs.Rotation, alpha),
			Lerp(lhs.Scale, rhs.Scale, alpha));
	}

	FORCEINLINE float Dot(Vector2f lhs, Vector2f rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y;
	}

	FORCEINLINE Vector2f Slerp2D(Vector2f n1, Vector2f n2, float t) // 두 벡터 모두 노말라이즈 돼있어야 함
	{
		float dot = Dot(n1, n2);
		dot = dot < -1 ? -1 : dot;
		dot = dot > 1 ? 1 : dot;

		if (dot > 0.999) // 방향이 거의 같으면
		{
			return n1;
		}

		float theta = acos(dot);
		float sinTheta = sin(theta);


		return (sin((1.0f - t) * theta) / sinTheta) * n1 + (sin(t * theta) / sinTheta) * n2;
	}

	FORCEINLINE float CrossMagnitute3D(const Vector4f& lhs, const Vector4f& rhs)
	{
		XMVECTOR Cross = XMVector3Cross(lhs.SimdVec, rhs.SimdVec);
		return XMVector3Length(Cross).m128_f32[0];
	}



	// CurPos의 영향력을 받는 v1, v2, v3의 가중치가 각각 x, y, z값으로 들어옴
	FORCEINLINE Vector4f CalcBarycentricWeight(Vector4f CurPos, Vector4f v1, Vector4f v2, Vector4f v3)
	{
		float WholeWidth = CrossMagnitute3D(v1 - v3, v2 - v3);

		float v1Width = CrossMagnitute3D(v2 - CurPos, v3 - CurPos);
		float v2Width = CrossMagnitute3D(v1 - CurPos, v3 - CurPos);
		float v3Width = CrossMagnitute3D(v1 - CurPos, v2 - CurPos);

		Vector4f Result = { v1Width, v2Width, v3Width , 0 };

		return Result / WholeWidth;
	}
};