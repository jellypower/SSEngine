#pragma once
#include "SSCollision/Public/CollisionBase/ICollisionWorld.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"


constexpr int32 COLLWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 COLLWORLD_BUCKET_CAPACITY = 1024;


class CollisionWorld : public ICollisionWorld
{
private:
	SS::SHasherW _WorldName;
	SS::HashMap<SObjHashCode, ICollInstanceBase*> _CollInstanceByHashCode;

public:
	CollisionWorld(const SS::SHasherW& worldName);

	bool IsAnyInstanceRemainInWorld() const override;
	SS::SHasherW GetWorldName() const override;
	void AddToWorld(ICollInstanceBase* InRenderInstance) override;
	void RemoveFromWorld(SObjHashCode CollInstanceIDToRemove) override;


	// DEBUG
private:
	SS::PooledList<CDDD_Line> _DDDListLine;
	SS::PooledList<CDDD_Mesh> _DDDListMesh;

public:
	virtual const SS::PooledList<CDDD_Line>& GetDDDList_Line() const override;
	virtual const SS::PooledList<CDDD_Mesh>& GetDDDList_Mesh() const override;

	virtual void FlushDDDList() override;

	virtual void AddDrawDebugLine(const CDDD_Line& Desc) override;
	virtual void AddDrawDebugMesh(const CDDD_Mesh& Desc) override;
	// ~DEBUG
};
