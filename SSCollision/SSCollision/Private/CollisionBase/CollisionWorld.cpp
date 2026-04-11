#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

CollisionWorld::CollisionWorld(const SS::SHasherW& worldName) :
	_WorldName(worldName),
	_CollInstanceByHashCode(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY),
	_TransformCommitNeededObjs(1024, 256),
	_RigidBodyByHashCode(1024, 256)
{
}

bool CollisionWorld::IsAnyInstanceRemainInWorld() const
{
	return
	_CollInstanceByHashCode.GetCnt() != 0 || 
		_RigidBodyByHashCode.GetCnt() != 0;
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


	bool bResult = _RigidBodyByHashCode.Remove(CollInstanceIDToRemove);
	// 없을 수도 있음. 없으면 bResult는 false
}

void CollisionWorld::AddToWorld(IRigidBodyBase* InRenderInstance)
{
	const ICollInstanceBase* CollInstance = InRenderInstance->GetCollInstance();

	SObjHashCode GOID = CollInstance->GetGameObjectID();
	if (_CollInstanceByHashCode.Find(GOID) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RigidBodyByHashCode.Add(GOID, InRenderInstance);
	InRenderInstance->OnEnterTheCollWorld(this);
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

void CollisionWorld::SimulateMovement(float DeltaTime)
{
	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode) // 일단 움직입니다. -> 병렬화 가능
	{
		IRigidBodyBase* RigidBodyItem = Item.second;
		RigidBodyItem->SimulateTick(DeltaTime);
	}

	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode) // 움직임을 반영합니다. -> 병렬화 가능
	{
		IRigidBodyBase* RigidBodyItem = Item.second;
		if (RigidBodyItem->IsMovedOnThisTick() == false)
		{
			continue;
		}

		ICollInstanceBase* ColItem = RigidBodyItem->GetCollInstance();

		bool bAnyMove = false;
		if (RigidBodyItem->IsMovedOnThisTick())
		{
			bAnyMove = true;
			ColItem->CollProcess_MoveObjecet(RigidBodyItem->GetSimulatedPosDelta());
		}

		if (RigidBodyItem->IsRotatedOnThisTick())
		{
			bAnyMove = true;
			ColItem->CollProcess_RotateObjecet(RigidBodyItem->GetSimulatedRotDelta());
		}

		if (bAnyMove)
		{
			ColItem->CommitTransform();
		}
	}

	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode)
	{
		IRigidBodyBase* RigidBodyItem = Item.second;
		RigidBodyItem->OnEndSimulation();
	}
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
