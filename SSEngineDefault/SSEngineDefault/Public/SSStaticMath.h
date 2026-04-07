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

	FORCEINLINE Vector4f Cross(const Vector4f& lhs, const Vector4f& rhs)
	{
		return XMVector3Cross(lhs.SimdVec, rhs.SimdVec);
	}


	FORCEINLINE float abs(float InValue)
	{
		return InValue < 0 ? -InValue : InValue;
	}


	FORCEINLINE Quaternion Slerp(const Quaternion& lhs, const Quaternion& rhs, float alpha)
	{
		return Quaternion(XMQuaternionSlerp(lhs.SimdVec, rhs.SimdVec, alpha));
	}

	FORCEINLINE float Lerp(float lhs, float rhs, float alpha)
	{
		return (rhs - lhs) * alpha + lhs;
	}
	FORCEINLINE Vector2f Lerp(Vector2f lhs, Vector2f rhs, float alpha)
	{
		return (rhs - lhs) * alpha + lhs;
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
		float Yaw1 = atan2(n1.Y, n1.X);
		float Yaw2 = atan2(n2.Y, n2.X);
		Yaw1 += XM_2PI;
		Yaw2 += XM_2PI;
		Yaw1 = fmod(Yaw1, XM_2PI);
		Yaw2 = fmod(Yaw2, XM_2PI);
		float Diff = Yaw2 - Yaw1;

		if (Diff > XM_PI) // ex) Yaw1=0 to Yaw2=270
		{
			Yaw1 += XM_2PI;
		}
		else if (Diff < -XM_PI) // ex) Yaw1=270 to Yaw2=0
		{
			Yaw2 += XM_2PI;
		}

		float NewYaw = SS::Lerp(Yaw1, Yaw2, t);

		return { cosf(NewYaw), sinf(NewYaw) };
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


	FORCEINLINE float CalcBiggestScaleAxis(const XMMATRIX& InMat)
	{
		XMVECTOR Axis = {
			InMat.r[0].m128_f32[0],
			InMat.r[1].m128_f32[0],
			InMat.r[2].m128_f32[0],
			InMat.r[3].m128_f32[0]
		};
		float xAxisSqr = XMVector3LengthSq(Axis).m128_f32[0];


		Axis = {
			InMat.r[0].m128_f32[1],
			InMat.r[1].m128_f32[1],
			InMat.r[2].m128_f32[1],
			InMat.r[3].m128_f32[1]
		};
		float yAxisSqr = XMVector3LengthSq(Axis).m128_f32[0];


		Axis = {
	InMat.r[0].m128_f32[2],
	InMat.r[1].m128_f32[2],
	InMat.r[2].m128_f32[2],
	InMat.r[3].m128_f32[2]
		};
		float zAxisSqr = XMVector3LengthSq(Axis).m128_f32[0];


		float GreatesAxisSqr = xAxisSqr > yAxisSqr ? xAxisSqr : yAxisSqr;
		GreatesAxisSqr = GreatesAxisSqr > zAxisSqr ? GreatesAxisSqr : zAxisSqr;
		return sqrt(GreatesAxisSqr);
	}
};