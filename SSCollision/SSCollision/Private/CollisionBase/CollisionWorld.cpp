#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

CollisionWorld::CollisionWorld(const SS::SHasherW& worldName) :
	_WorldName(worldName),
	_CollInstanceByHashCode(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY),
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

void CollisionWorld::UpdateInitialTransforms()
{
	for (SS::pair<SObjHashCode, IRigidBodyBase*>& PairItem : _RigidBodyByHashCode)
	{
		ICollInstanceBase* ICI = PairItem.second->GetCollInstance();
		PairItem.second->UpdateInitialTransform(ICI->GetWorldPos(), ICI->GetWorldRot());
	}
}


void CollisionWorld::OnBeginSimulation()
{
	UpdateInitialTransforms();
}

void CollisionWorld::SimulateMovement(float DeltaTime)
{
	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode) // 일단 움직이고 움직임을 반영합니다.
	{
		IRigidBodyBase* RigidBodyItem = Item.second;
		RigidBodyItem->SimulateMovement(DeltaTime);

		ICollInstanceBase* ColItem = RigidBodyItem->GetCollInstance();

		if (RigidBodyItem->IsMovedOnThisSimulation())
		{
			ColItem->CollProcess_MoveObjecet(RigidBodyItem->GetSimulatedPosDelta());
			// 일단 움직이고 나서 충돌체크 하는지 검사하려면 실제 Collision을 바꿔줘야 함
		}

		if (RigidBodyItem->IsRotatedOnThisSimulation())
		{
			ColItem->CollProcess_RotateObjecet(RigidBodyItem->GetSimulatedRotDelta());
			// 일단 움직이고 나서 충돌체크 하는지 검사하려면 실제 Collision을 바꿔줘야 함
		}
	}

	// TODO: 2. 충돌하는 pair들을 찾습니다.
	// TODO: 3. Solve 합니다.


}

void CollisionWorld::OnEndSimulation()
{
	// 원본 위치와 다르면 GameObejct가 SetTransform할 수 있도록 유도해줘야 한다.
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
