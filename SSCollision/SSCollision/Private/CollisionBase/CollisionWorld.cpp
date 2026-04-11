#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"

CollisionWorld::CollisionWorld(const SS::SHasherW& worldName)
	: _WorldName(worldName),
	_CollInstanceByHashCode(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY),
	_TransformCommitNeededObjs(1024, 256)
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

void CollisionWorld::AddToWorld(IRigidBodyBase* InRenderInstance)
{
	SS_ASSERT(false); // TODO: Impl
}

void CollisionWorld::ProcessTransformCommit()
{
	for (SS::pair<SObjHashCode, ICollInstanceBase*>& PairItem : _TransformCommitNeededObjs)
	{
		PairItem.second->CommitTransform();
	}

	_TransformCommitNeededObjs.Clear();
}

void CollisionWorld::AddTransformCommitNeededObj(ICollInstanceBase* InCollInstance)
{
	if (InCollInstance->GetIncludedCollWorld() != this)
	{
		SS_INTERRUPT(false);
		return;
	}

	SObjHashCode GOID = InCollInstance->GetGameObjectID();
	ICollInstanceBase** ppFound = _TransformCommitNeededObjs.Find(GOID);
	if (ppFound != nullptr)
	{
		return; // 부모 오브젝트의 위치가 업데이트 되면서 자식 오브젝트를 포함시켰으면 이미 존재할 수도 있음
	}

	_TransformCommitNeededObjs.Add(GOID, InCollInstance);
}

const SS::PooledList<CDDD_Line>& CollisionWorld::GetDDDList_Line() const
{
	return _DDDListLine;
}

const SS::PooledList<CDDD_Mesh>& CollisionWorld::GetDDDList_Mesh() const
{
	return _DDDListMesh;
}

void CollisionWorld::FlushDDDList()
{
	_DDDListLine.Clear();
	_DDDListMesh.Clear();
}

void CollisionWorld::AddDrawDebugLine(const CDDD_Line& Desc)
{
	_DDDListLine.PushBack(Desc);
}

void CollisionWorld::AddDrawDebugMesh(const CDDD_Mesh& Desc)
{
	_DDDListMesh.PushBack(Desc);
}
