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

};
