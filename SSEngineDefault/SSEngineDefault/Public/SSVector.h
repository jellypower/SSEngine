#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSDirectXMathCustom.h"
#include "SSNativeKeywords.h"

using namespace DirectX;

struct Quaternion;
struct Vector4f;

struct SSENGINEDEFAULT_MODULE Vector4f
{
	union
	{
		XMVECTOR SimdVec;

		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};
		float At[4];
	};

	Vector4f() : SimdVec({ 0 }) { }
	Vector4f(XMVECTOR InXMVECTOR) : SimdVec(InXMVECTOR) { }
	Vector4f(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }

	inline Vector4f Get3DNormalized() const;
	inline float Get3DLength() const;
	inline float Get3DSqrLength() const;
	inline Vector4f Rotate(Quaternion Rotation) const;

	static const Vector4f Forward;
	static const Vector4f Back;
	static const Vector4f Up;
	static const Vector4f Down;
	static const Vector4f Left;
	static const Vector4f Right;
	static const Vector4f Zero;
	static const Vector4f One;
	static const Vector4f Half;
};

FORCEINLINE Vector4f operator+(const Vector4f& lhs, const Vector4f& rhs) { return lhs.SimdVec + rhs.SimdVec; }
FORCEINLINE Vector4f operator-(const Vector4f& lhs, const Vector4f& rhs) { return lhs.SimdVec - rhs.SimdVec; }
FORCEINLINE Vector4f operator*(const Vector4f& lhs, const Vector4f& rhs) { return lhs.SimdVec * rhs.SimdVec; }
FORCEINLINE Vector4f operator*(const Vector4f& lhs, float rhs) { return lhs.SimdVec * rhs; }
FORCEINLINE Vector4f operator*(float lhs, const Vector4f& rhs) { return lhs * rhs.SimdVec; }
FORCEINLINE Vector4f operator/(const Vector4f& lhs, float rhs) { return lhs.SimdVec / rhs; }
FORCEINLINE Vector4f operator-(const Vector4f& inVal) { return -inVal.SimdVec; }

inline Vector4f Vector4f::Get3DNormalized() const { return SimdVec / Get3DLength(); }
inline float Vector4f::Get3DLength() const { return sqrt(Get3DSqrLength()); }
inline float Vector4f::Get3DSqrLength() const { return XMVector3LengthSq(SimdVec).m128_f32[0]; }




struct SSENGINEDEFAULT_MODULE Vector2f
{
	float X;
	float Y;

	Vector2f() : X(0), Y(0) { }
	Vector2f(float InX, float InY) : X(InX), Y(InY) { }

	inline float GetSqrLength() const;

	static const Vector2f Zero;
	static const Vector2f One;
};

FORCEINLINE Vector2f operator-(const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(lhs.X - rhs.X, lhs.Y - rhs.Y); }

float Vector2f::GetSqrLength() const { return (X * X) + (Y * Y); }



struct SSENGINEDEFAULT_MODULE Quaternion
{
	union
	{
		XMVECTOR SimdVec;
		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};
		float At[4];
	};

	Quaternion() : X(0), Y(0), Z(0), W(1) { }
	Quaternion(__m128 InSimdVector) : SimdVec(InSimdVector) { }
	FORCEINLINE XMMATRIX AsMatrix() const { return XMMatrixRotationQuaternion(SimdVec); }
	FORCEINLINE XMMATRIX AsInverseMatrix() const { return XMMatrixRotationQuaternion(XMQuaternionInverse(SimdVec)); }
	FORCEINLINE Quaternion operator*(Quaternion lhs) const { return Quaternion(XMQuaternionMultiply(SimdVec, lhs.SimdVec)); }

	FORCEINLINE Quaternion Inverse() const;

	FORCEINLINE static Quaternion FromEulerRotation(Vector4f eulerRotation);
	FORCEINLINE static Quaternion FromLookDirect(Vector4f lookDirection, Vector4f upDirection = Vector4f::Up);
	FORCEINLINE static Quaternion RotateAxisAngle(Quaternion CurRotation, Vector4f Axis, float angle);
};

Quaternion Quaternion::FromEulerRotation(Vector4f eulerRotation)
{
	//	<Pitch, Yaw, Roll, 0>
	return XMQuaternionRotationRollPitchYawFromVector(eulerRotation.SimdVec);
}

Quaternion Quaternion::FromLookDirect(Vector4f lookDirection, Vector4f upDirection)
{
	return XMQuaternionRotationMatrix(
		XMMatrixLookToLH(Vector4f::Zero.SimdVec, lookDirection.SimdVec, upDirection.SimdVec));
}

Quaternion Quaternion::RotateAxisAngle(Quaternion CurRotation, Vector4f Axis, float angle)
{
	return XMQuaternionMultiply(CurRotation.SimdVec, XMQuaternionRotationAxis(Axis.SimdVec, angle));
}


struct SSENGINEDEFAULT_MODULE Vector2i32 {
	int32 X;
	int32 Y;

	Vector2i32() : X(0), Y(0) {}
	Vector2i32(int32 InX, int32 InY) : X(InX), Y(InY) {}

	static const Vector2i32 Zero;
};


FORCEINLINE Vector2i32 operator+(const Vector2i32 lhs, const Vector2i32 rhs) { return Vector2i32(lhs.X + rhs.X, lhs.Y + rhs.Y); }
FORCEINLINE Vector2i32 operator-(const Vector2i32 lhs, const Vector2i32 rhs) { return Vector2i32(lhs.X - rhs.X, lhs.Y - rhs.Y); }



struct Vector2ui32 {
	uint32 X;
	uint32 Y;

	Vector2ui32() : X(0), Y(0) { }
	Vector2ui32(uint32 InX, uint32 InY) : X(InX), Y(InY) { }
};




struct SSENGINEDEFAULT_MODULE Transform {
	Vector4f Position;
	Quaternion Rotation;
	Vector4f Scale;

	Transform() : Position(), Rotation(), Scale(1, 1, 1, 0) { }
	Transform(Vector4f InPos, Quaternion InRot, Vector4f InScale) : Position(InPos), Rotation(InRot), Scale(InScale) { }


	FORCEINLINE XMMATRIX AsMatrix() const;
	FORCEINLINE XMMATRIX AsInverseMatrix() const;
	FORCEINLINE Transform Inverse() const;

	/*
	 * 현재 transform을 other을 곱해서 변형시킴
	 */
	FORCEINLINE Transform operator*(const Transform& other) const;


	FORCEINLINE Vector4f GetForward() const
	{
		return XMVector3Rotate(Vector4f::Forward.SimdVec, Rotation.SimdVec);
	};
	FORCEINLINE Vector4f GetBackward() const
	{
		return XMVector3Rotate(Vector4f::Back.SimdVec, Rotation.SimdVec);
	}
	FORCEINLINE Vector4f GetUp() const
	{
		return XMVector3Rotate(Vector4f::Up.SimdVec, Rotation.SimdVec);
	}
	FORCEINLINE Vector4f GetDown() const
	{
		return XMVector3Rotate(Vector4f::Down.SimdVec, Rotation.SimdVec);
	}
	FORCEINLINE Vector4f GetLeft() const
	{
		return XMVector3Rotate(Vector4f::Left.SimdVec, Rotation.SimdVec);
	}
	FORCEINLINE Vector4f GetRight() const
	{
		return XMVector3Rotate(Vector4f::Right.SimdVec, Rotation.SimdVec);
	}

	static const Transform Identity;
};


#pragma region inline implementation

Vector4f Vector4f::Rotate(Quaternion Rotation) const { return XMVector3Rotate(SimdVec, Rotation.SimdVec); }


XMMATRIX Transform::AsMatrix() const
{
	return
		XMMatrixAffineTransformation(
			Scale.SimdVec,		// 스케일
			{ 0 },	// 피벗
			Rotation.SimdVec,	// 회전
			Position.SimdVec	// 위치
		);
}

XMMATRIX Transform::AsInverseMatrix() const
{
	XMMATRIX MScaling = XMMatrixScalingFromVector(Scale.SimdVec);

	XMMATRIX M = XMMatrixRotationQuaternion(Rotation.SimdVec);
	M.r[3] = XMVectorAdd(M.r[3], Position.SimdVec);
	M = InverseRigid(M);
	M = XMMatrixMultiply(M, MScaling);
	return M;
}

Transform Transform::Inverse() const
{
	Transform inverse = Transform::Identity;
	const Quaternion InverseRot = Rotation.Inverse();
	const Vector4f InverseScale = Vector4f(1 / Scale.X, 1 / Scale.Y, 1 / Scale.Z, 0);

	inverse.Position = (-Position).Rotate(InverseRot) * InverseScale;
	inverse.Rotation = InverseRot;
	inverse.Scale = InverseScale;

	return inverse;
}

Transform Transform::operator*(const Transform& other) const
{
	Transform Result = Transform::Identity;

	Result.Rotation = Rotation * other.Rotation;
	Result.Scale = Scale * other.Scale;
	Result.Position = (Position * other.Scale).Rotate(other.Rotation) + other.Position;

	return Result;
}


Quaternion Quaternion::Inverse() const
{
	return Quaternion(XMQuaternionInverse(SimdVec));
}

#pragma endregion

