#pragma once
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"


class IRigidBodyBase;
class ICollInstanceBase;

class ICollisionWorld : public INoncopyable
{
public:
	virtual void FinalizeCollWorld() = 0;

	virtual bool IsAnyInstanceRemainInWorld() const = 0;
	virtual SS::SHasherW GetWorldName() const = 0;
	virtual const SS::HashMap<SObjHashCode, IRigidBodyBase*>& GetRigidBodyByHashCode() const = 0;


	virtual void QueryCollidableWith(SS::PooledList<ICollInstanceBase*>& OutList, ICollInstanceBase* CollTarget) const = 0;

	virtual void AddToWorld(ICollInstanceBase* InCollInstance) = 0;
	virtual void AddToWorld(IRigidBodyBase* InRigidBody) = 0;
	virtual void RemoveCollFromWorld(ICollInstanceBase* InCollInstance) = 0;
	virtual void RemoveRigidFromWorld(IRigidBodyBase* InRigidBody) = 0;


	// Transform Commit
	virtual void UpdateInitialTransforms() = 0;


	// Simulate
	virtual void OnBeginSimulation() = 0;
	virtual void SimulateMovement(float DeltaTime) = 0;
	virtual void OnEndSimulation() = 0;



	// DEBUG
public:
	virtual const SS::PooledList<CDDD_Line>& GetDDDList_Line() const = 0;
	virtual const SS::PooledList<CDDD_Mesh>& GetDDDList_Mesh() const = 0;

	virtual void FlushDDDList() = 0;

	virtual void AddDrawDebugLine(const CDDD_Line& Desc) = 0;
	virtual void AddDrawDebugMesh(const CDDD_Mesh& Desc) = 0;
	// ~DEBUG
};
