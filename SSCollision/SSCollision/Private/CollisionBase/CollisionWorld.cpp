#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"

CollisionWorld::CollisionWorld(const SS::SHasherW& worldName)
	: _WorldName(worldName),
	_CollInstanceByHashCode(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY)
{
}

bool CollisionWorld::IsAnyInstanceRemainInWorld() const
{
	return _CollInstanceByHashCode.GetCnt() != 0;
}

SS::SHasherW CollisionWorld::GetWorldName() const
{
	return _WorldName;
}

void CollisionWorld::AddToWorld(ICollInstanceBase* InRenderInstance)
{
	SObjHashCode GOID = InRenderInstance->GetGameObjectID();
	if (_CollInstanceByHashCode.Find(GOID) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_CollInstanceByHashCode.Add(GOID, InRenderInstance);
	InRenderInstance->OnEnterTheCollWorld(this);
}

void CollisionWorld::RemoveFromWorld(SObjHashCode CollInstanceIDToRemove)
{
	ICollInstanceBase** ppCollInstance = _CollInstanceByHashCode.Find(CollInstanceIDToRemove);
	if (ppCollInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	ICollInstanceBase* CollInstanceToRemove = *ppCollInstance;
	if (CollInstanceToRemove == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_CollInstanceByHashCode.Remove(CollInstanceIDToRemove);
	CollInstanceToRemove->OnExitFromCollWorld();
}
