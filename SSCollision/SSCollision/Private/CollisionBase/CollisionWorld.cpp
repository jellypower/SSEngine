#include "pch.h"
#include "CollisionWorld.h"

#include "SSCollision/Private/CollInstance/CIUtils_Private.h"
#include "SSCollision/Private/RigidBody/RIUtils_Private.h"
#include "SSCollision/Private/SpatialSystem/SASSweepAndPrune.h"
#include "SSCollision/Public/CollInstance/ICollInstanceBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyBase.h"
#include "SSCollision/Public/RigidBody/IRigidBodyCustomSim.h"
#include "SSCollision/Public/RigidBody/IRigidBodyDynamic.h"
#include "SSCollision/Public/RigidBody/RIUtils_Public.h"


CollisionWorld::CollisionWorld(const SS::SHasherW& worldName, physx::PxScene* PhysxScene) :
	_StaticRigidBodies(COLLWORLD_HASHMAP_SIZE, COLLWORLD_BUCKET_CAPACITY),
	_DynamicRigidBodies(1024, 128),
	_CustomSimBodies(1024, 128)
{
	_PhysXScene = PhysxScene;
	_WorldName = worldName;
}

void CollisionWorld::Release()
{
	PX_RELEASE(_PhysXScene);

	//	if (_SASSweepAndPruen->IsAnyInstanceExists())
	//	{
	//		SS_INTERRUPT();
	//	}
	//
	//	delete _SASSweepAndPruen;

	delete this;
}


void CollisionWorld::FinalizeCollWorld()
{
//	_SASSweepAndPruen->FinalizePendingInstances();
}

bool CollisionWorld::IsAnyInstanceRemainInWorld() const
{
	return
		_StaticRigidBodies.GetCnt() != 0 ||
		_DynamicRigidBodies.GetCnt() != 0 ||
		_CustomSimBodies.GetCnt() != 0;
}

const SS::HashMap<SObjHashCode, IRigidBodyBase*>& CollisionWorld::GetStaticRigidBodies() const
{
	return _StaticRigidBodies;
}

const SS::HashMap<SObjHashCode, IRigidBodyDynamic*>& CollisionWorld::GetDynamicRigidBodies() const
{
	return _DynamicRigidBodies;
}

const SS::HashMap<SObjHashCode, IRigidBodyCustomSim*>& CollisionWorld::GetCustomSimBodies() const
{
	return _CustomSimBodies;
}

SS::SHasherW CollisionWorld::GetWorldName() const
{
	return _WorldName;
}


void CollisionWorld::QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList,
	ICollInstanceBase* CollTarget) const
{
//	_SASSweepAndPruen->QueryCollidableWith(OutList, CollTarget);
}


void CollisionWorld::AddToWorld(IRigidBodyBase* InRigidBody)
{
	const SObjHashCode GOID = InRigidBody->GetGameObjectID();
	const ERigidBodyType RIType = InRigidBody->GetRigidBodyType();
	const ERigidBodySimType SimType = GetRigidBodySimType(RIType);

	if (SimType == ERigidBodySimType::Static)
	{
		if (_StaticRigidBodies.Find(GOID) != nullptr)
		{
			SS_ASSERT(false);
			return;
		}

		_StaticRigidBodies.Add(GOID, InRigidBody);
	}
	else if (SimType == ERigidBodySimType::Dynamic)
	{
		if (_DynamicRigidBodies.Find(GOID) != nullptr)
		{
			SS_ASSERT(false);
			return;
		}

		_DynamicRigidBodies.Add(GOID, static_cast<IRigidBodyDynamic*>(InRigidBody));
	}
	else if (SimType == ERigidBodySimType::CustomSim)
	{
		if (_CustomSimBodies.Find(GOID) != nullptr)
		{
			SS_ASSERT(false);
			return;
		}

		_CustomSimBodies.Add(GOID, static_cast<IRigidBodyCustomSim*>(InRigidBody));
	}
	else
	{
		SS_ASSERT(false);
		return;
	}

	physx::PxActor* PxActor = ExtractPxActor(InRigidBody);
	_PhysXScene->addActor(*PxActor);
	InRigidBody->OnEnterTheCollWorld(this);
}



void CollisionWorld::RemoveRigidFromWorld(IRigidBodyBase* InRigidBody)
{
	const SObjHashCode GOID = InRigidBody->GetGameObjectID();
	const ERigidBodyType RIType = InRigidBody->GetRigidBodyType();
	const ERigidBodySimType SimType = GetRigidBodySimType(RIType);

	IRigidBodyBase* Found = nullptr;

	if (SimType == ERigidBodySimType::Static)
	{
		bool bResult = _StaticRigidBodies.Remove(GOID);
		if (bResult == false)
		{
			SS_INTERRUPT();
		}
	}
	else if (SimType == ERigidBodySimType::Dynamic)
	{
		bool bResult = _DynamicRigidBodies.Remove(GOID);
		if (bResult == false)
		{
			SS_INTERRUPT();
		}
	}
	else if (SimType == ERigidBodySimType::CustomSim)
	{
		bool bResult = _CustomSimBodies.Remove(GOID);
		if (bResult == false)
		{
			SS_INTERRUPT();
		}
	}
	else
	{
		SS_ASSERT(false);
		return;
	}


	physx::PxActor* PxActor = ExtractPxActor(InRigidBody);
	_PhysXScene->removeActor(*PxActor);
	InRigidBody->OnExitFromCollWorld();
}


void CollisionWorld::OnBeginSimulation()
{
	for (SS::pair<SObjHashCode, IRigidBodyCustomSim*>& item : _CustomSimBodies)
	{
		item.second->OnBeginSimulation();
	}

	for (SS::pair<SObjHashCode, IRigidBodyDynamic*>& item : _DynamicRigidBodies)
	{
		item.second->OnBeginSimulation();
	}
}

void CollisionWorld::SimulateMovement(float DeltaTime)
{
	for (SS::pair<SObjHashCode, IRigidBodyCustomSim*>& item : _CustomSimBodies)
	{
		item.second->SimulateMovement(DeltaTime);
	}

	_PhysXScene->simulate(DeltaTime);
	_PhysXScene->fetchResults(true);

}

void CollisionWorld::OnEndSimulation()
{
	for (SS::pair<SObjHashCode, IRigidBodyCustomSim*>& item : _CustomSimBodies)
	{
		item.second->OnEndSimulation();
	}

	for (SS::pair<SObjHashCode, IRigidBodyDynamic*>& item : _DynamicRigidBodies)
	{
		item.second->OnEndSimulation();
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
