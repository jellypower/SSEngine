#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIBox.h"
#include "SSCollision/Private/CollDetect/CollCalc_Private.h"
#include "SSCollision/Private/CollInstance/CISphere.h"
#include "SSCollision/Private/RigidBody/RigidCharacterMovement.h"

ICollisionWorld* CollDevice::CreateCollWorld(SS::SHasherW InWorldName) const
{
	return DBG_NEW CollisionWorld(InWorldName);
}

ICIBox* CollDevice::CreateCollBox()
{
	return DBG_NEW CIBox();
}

ICISphere* CollDevice::CreateCollSphere()
{
	return DBG_NEW CISphere();
}

IRigidCahracterMovement* CollDevice::CreateCharacterMovement()
{
	return DBG_NEW RigidCharacterMovement();
}

bool CollDevice::AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2)
{
	Vector4f BBMinC1 = c1->GetBBMin();
	Vector4f BBMaxC1 = c1->GetBBMax();

	Vector4f BBMinC2 = c2->GetBBMin();
	Vector4f BBMaxC2 = c2->GetBBMax();

	XMVECTOR Result1 = XMVectorGreaterOrEqual(BBMinC2.SimdVec, BBMaxC1.SimdVec);
	XMVECTOR Result2 = XMVectorGreaterOrEqual(BBMinC1.SimdVec, BBMaxC2.SimdVec);
	XMVECTOR Result = XMVectorSetW(XMVectorOrInt(Result1, Result2), 0);

	if (_mm_movemask_ps(Result) != 0)
	{
		return false;
	}


	return CollCalc_Private::GJK(c1, c2);
}
