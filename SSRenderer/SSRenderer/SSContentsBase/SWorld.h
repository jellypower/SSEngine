#pragma once
#include "SObject/Public/SObjConstructor.h"
#include "SObject/Public/SObjectBase.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"


class RenderWorld;
constexpr int32 WORLD_OBJECTMAP_HASHMAP_SIZE = 1024 * 16;
constexpr int32 WORLD_OBJECTMAP_HASHBUCKET_SIZE = 512;

constexpr int32 TRANSFORM_UPDATE_HASHMAP_SIZE = 1024;
constexpr int32 TRANSFORM_UPDATE_HASHBUCKET_SIZE = 256;

class SGameObject;

class SWorld : public SObjectBase
{
private:
	SGameObject* _WorldRootObject = nullptr;
	SS::HashMap<SObjHashCode, SGameObject*> _ObjectsByHashCode;
	SS::HashMap<SObjHashCode, SGameObject*> _ObjectsNeedToUpdateTransform; // TODO: 업데이트 끝나면 Clear해줘야함

	RenderWorld* _RenderWorld = nullptr;

public:
	SWorld();
	virtual ~SWorld();
	void PostConstruct() override;
	void PreDestruct() override;
	virtual void InitializeWorld(RenderWorld* InRenderWorld);

public:
	RenderWorld* GetRenderWorld() const { return _RenderWorld; }
	SGameObject* GetWorldRootObject() const { return _WorldRootObject; }
	bool IsAnyObjectRemainInWorld() const;

	void AddToWorld(SGameObject* InNewWorldObject, SGameObject* ParentObject = nullptr);
	void RemoveFromWorld(SGameObject* InObjectToRemove);
	void DestroyAllObjectsInWorld();

	void ClearObjectsNeedToUpdateTransformList();
	void AddObjectNeedToUpdateTransform(SGameObject* InObj);

private:
	void AddToWorld_Recursion(SGameObject* InNewObject);
	void RemoveFromWorld_Recursion(SGameObject* InObjectToRemove);

	void AddGameObjectItem(SGameObject* InNewObject);
	void RemoveGameObjectItem(SGameObject* InObjectToRemove);

private:
	void AddWorldRootObject(SGameObject* InWorldRootObject);
};
