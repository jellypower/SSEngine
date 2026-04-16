#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIBox.h"
#include "SSCollision/Private/CollDetect/CollCalc_Private.h"
#include "SSCollision/Private/CollInstance/CISphere.h"
#include "SSCollision/Private/RigidBody/RigidCharacterMovement.h"

#include "SSEngineDefault/Public/Collision/CollMathInline.h"

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
	const AABBBox& BBoxC1 = c1->GetBBox();
	const AABBBox& BBoxC2 = c2->GetBBox();


	if (CollMath_Inline::BBIntersect(BBoxC1, BBoxC2) == false)
	{
		return false;
	}


	return CollCalc_Private::GJK(c1, c2);
}
