#include "pch.h"
#include "CollDevice.h"

#include "CollisionWorld.h"
#include "SSCollision/Private/CollInstance/CIBox.h"

ICollisionWorld* CollDevice::CreateCollWorld(SS::SHasherW InWorldName) const
{
	return DBG_NEW CollisionWorld(InWorldName);
}

ICIBox* CollDevice::CreateCollBox()
{
	return DBG_NEW CIBox();
}
