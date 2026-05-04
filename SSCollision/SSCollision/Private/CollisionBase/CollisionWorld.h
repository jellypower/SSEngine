#pragma once
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"


namespace physx
{
	class PxScene;
}

class SASSweepAndPrune;
constexpr int32 COLLWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 COLLWORLD_BUCKET_CAPACITY = 1024;


class CollisionWorld : public ICollisionWorld
{
private:
	SS::SHasherW _WorldName;
	SS::HashMap<SObjHashCode, ICollInstanceBase*> _CollInstanceByHashCode;
	SS::HashMap<SObjHashCode, IRigidBodyBase*> _RigidBodyByHashCode;
	SS::HashMap<physx::PxRigidActor*, SObjHashCode> _HashCodeByRigidActor;



	physx::PxScene* _PhysXScene = nullptr;


public:
	CollisionWorld(const SS::SHasherW& worldName, physx::PxScene* PhysxScene);
	virtual ~CollisionWorld();

public:
	void FinalizeCollWorld() override;

	// Add Remove From World
	bool IsAnyInstanceRemainInWorld() const override;
	SS::SHasherW GetWorldName() const override;
	const SS::HashMap<SObjHashCode, IRigidBodyBase*>& GetRigidBodyByHashCode() const override;

	void QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const override;

	void AddToWorld(ICollInstanceBase* InCollInstance) override;
	void AddToWorld(IRigidBodyBase* InRigidBody) override;
	void RemoveCollFromWorld(ICollInstanceBase* InCollInstance) override;
	void RemoveRigidFromWorld(IRigidBodyBase* InRigidBody) override;
	



	// Transform Commit
	virtual void UpdateInitialTransforms() override;

	// Simulate
	void OnBeginSimulation() override;
	void SimulateMovement(float DeltaTime) override;
	void OnEndSimulation() override;



	// =============== DEBUG =============== 
private:
	SS::PooledList<CDDD_Line> _DDDListLine;
	SS::PooledList<CDDD_Mesh> _DDDListMesh;

public:
	virtual const SS::PooledList<CDDD_Line>& GetDDDList_Line() const override;
	virtual const SS::PooledList<CDDD_Mesh>& GetDDDList_Mesh() const override;

	virtual void FlushDDDList() override;

	virtual void AddDrawDebugLine(const CDDD_Line& Desc) override;
	virtual void AddDrawDebugMesh(const CDDD_Mesh& Desc) override;
	// =============== ~DEBUG =============== 
};
