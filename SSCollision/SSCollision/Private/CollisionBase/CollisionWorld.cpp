#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Private/SpatialSystem/SASSweepAndPrune.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"

CollisionWorld::CollisionWorld(const SS::SHasherW& worldName) :
	_WorldName(worldName),
	_CollInstanceByHashCode(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY),
	_RigidBodyByHashCode(1024, 256)
{
	_SASSweepAndPruen = DBG_NEW SASSweepAndPrune();
}

CollisionWorld::~CollisionWorld()
{
	if (_SASSweepAndPruen->IsAnyInstanceExists())
	{
		SS_INTERRUPT();
	}

	delete _SASSweepAndPruen;
}

void CollisionWorld::FinalizeCollWorld()
{
	_SASSweepAndPruen->FlushPendingInstances();
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

const SS::HashMap<SObjHashCode, IRigidBodyBase*>& CollisionWorld::GetRigidBodyByHashCode() const
{
	return _RigidBodyByHashCode;
}

void CollisionWorld::QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList,
	ICollInstanceBase* CollTarget) const
{
	_SASSweepAndPruen->QueryCollidableWith(OutList, CollTarget);
}

void CollisionWorld::AddToWorld(ICollInstanceBase* InCollInstance)
{
	SObjHashCode GOID = InCollInstance->GetGameObjectID();
	if (_CollInstanceByHashCode.Find(GOID) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_CollInstanceByHashCode.Add(GOID, InCollInstance);
	_SASSweepAndPruen->AddCollInstance(InCollInstance);
	InCollInstance->OnEnterTheCollWorld(this);
}

void CollisionWorld::AddToWorld(IRigidBodyBase* InRigidBody)
{
	SObjHashCode GOID = InRigidBody->GetGameObjectID();
	if (_RigidBodyByHashCode.Find(GOID) != nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	_RigidBodyByHashCode.Add(GOID, InRigidBody);
	InRigidBody->OnEnterTheCollWorld(this);
}

void CollisionWorld::RemoveCollFromWorld(ICollInstanceBase* InCollInstance)
{
	SObjHashCode InID = InCollInstance->GetGameObjectID();
	ICollInstanceBase** ppCollInstance = _CollInstanceByHashCode.Find(InID);
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

	SS_ASSERT(CollInstanceToRemove == InCollInstance);
	_CollInstanceByHashCode.Remove(InID);
	_SASSweepAndPruen->RemoveCollInstance(InCollInstance);
	CollInstanceToRemove->OnExitFromCollWorld();
}

void CollisionWorld::RemoveRigidFromWorld(IRigidBodyBase* InRigidBody)
{
	SObjHashCode InID = InRigidBody->GetGameObjectID();
	IRigidBodyBase** ppCollInstance = _RigidBodyByHashCode.Find(InID);
	if (ppCollInstance == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	IRigidBodyBase* RigidBodyToRemove = *ppCollInstance;
	if (RigidBodyToRemove == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	SS_ASSERT(RigidBodyToRemove == InRigidBody);
	_RigidBodyByHashCode.Remove(InID);
	InRigidBody->OnExitFromCollWorld();
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
	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode)
	{
		IRigidBodyBase* RigidBodyItem = Item.second;
		RigidBodyItem->OnBeginSimulation();
	}

	UpdateInitialTransforms();

	_SASSweepAndPruen->UpdateSAPStructure();
}

void CollisionWorld::SimulateMovement(float DeltaTime)
{
	// TODO: 1. 일단 움직이고 움직임을 반영합니다. 멀티스레드 가능
	for (SS::pair<SObjHashCode, IRigidBodyBase*> Item : _RigidBodyByHashCode)
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

	// TODO: 동기화
	// TODO: 2. 각 오브젝트 별로 본인과 충돌하는 오브젝트를 찾고 어디로 이동해야 할지 결정합니다. (멀티스레드 가능)


	// TODO: 동기화 
	// TODO: 3. 결정한 대로 오브젝트를 움직입니다. (멀티스레드 불가능)


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
