#pragma once
#include "SSCollision/Public/DEBUG/CollDebugDrawDescs.h"


class IRigidBodyBase;
class ICollInstanceBase;

class ICollisionWorld : public INoncopyable
{
public:
	virtual bool IsAnyInstanceRemainInWorld() const = 0;
	virtual SS::SHasherW GetWorldName() const = 0;

	// CollisionInstance Add Remove
	virtual void AddToWorld(ICollInstanceBase* InRenderInstance) = 0;
	virtual void RemoveFromWorld(SObjHashCode CollInstanceIDToRemove) = 0;

	// RigidBody Add Remove
	virtual void AddToWorld(IRigidBodyBase* InRenderInstance) = 0;


	// Transform Commit
	virtual void ProcessTransformCommit() = 0;
	virtual void AddTransformCommitNeededObj(ICollInstanceBase* InCollInstance) = 0;


	// DEBUG
public:
	virtual const SS::PooledList<CDDD_Line>& GetDDDList_Line() const = 0;
	virtual const SS::PooledList<CDDD_Mesh>& GetDDDList_Mesh() const = 0;

	virtual void FlushDDDList() = 0;

	virtual void AddDrawDebugLine(const CDDD_Line& Desc) = 0;
	virtual void AddDrawDebugMesh(const CDDD_Mesh& Desc) = 0;
	// ~DEBUG
};
