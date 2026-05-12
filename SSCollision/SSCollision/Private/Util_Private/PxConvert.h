#pragma once

// PhysX (right-handed, Z-out) <-> DirectX (left-handed, Z-in)
// Position : negate Z
// Quaternion: negate X, Y
class PxTransformConvert
{
public:
	FORCEINLINE static Vector4f Vec3FromPx(const physx::PxVec3& In)
	{
		return { In.x, In.y, -In.z, 1 };
	}

	FORCEINLINE static physx::PxVec3 Vec3ToPx(const Vector4f& In)
	{
		return { In.X, In.Y, -In.Z };
	}

	FORCEINLINE static Quaternion QuatFromPx(const physx::PxQuat& In)
	{
		return Quaternion(XMVectorSet(-In.x, -In.y, In.z, In.w));
	}

	FORCEINLINE static physx::PxQuat QuatToPx(const Quaternion& In)
	{
		return { -In.X, -In.Y, In.Z, In.W };
	}

	FORCEINLINE static Transform TransformFromPx(const physx::PxTransform& In)
	{
		return { Vec3FromPx(In.p), QuatFromPx(In.q), { 1, 1, 1, 0 } };
	}

	FORCEINLINE static physx::PxTransform TransformToPx(const Transform& In)
	{
		return { Vec3ToPx(In.Position), QuatToPx(In.Rotation) };
	}
};
