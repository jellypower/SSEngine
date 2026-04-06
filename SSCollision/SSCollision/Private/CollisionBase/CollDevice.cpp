#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIBox.h"
#include "SSCollision/Private/CollDetect/CollCalc_Private.h"

ICollisionWorld* CollDevice::CreateCollWorld(SS::SHasherW InWorldName) const
{
	return DBG_NEW CollisionWorld(InWorldName);
}

ICIBox* CollDevice::CreateCollBox()
{
	return DBG_NEW CIBox();
}

bool CollDevice::AreColliding(const ICollInstanceBase* c1, const ICollInstanceBase* c2)
{
	return CollCalc_Private::GJK(c1, c2);
}
