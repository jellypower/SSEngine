#pragma once
#include "SObject/Public/SObjectBase.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class SComponentBase;
class IRenderWorld;
class RenderWorld;
constexpr int32 WORLD_OBJECTMAP_HASHMAP_SIZE = 1024 * 16;
constexpr int32 WORLD_OBJECTMAP_HASHBUCKET_SIZE = 512;

constexpr int32 TRANSFORM_UPDATE_HASHMAP_SIZE = 1024;
constexpr int32 TRANSFORM_UPDATE_HASHBUCKET_SIZE = 256;

class SGameObject;

class SSCONTENTBASE_MODULE SWorld : public SObjectBase
{
private:
	SGameObject* _WorldRootObject = nullptr;
	SS::HashMap<SObjHashCode, SGameObject*> _ObjectsByHashCode;
	SS::HashMap<SObjHashCode, SComponentBase*> _FrameProcessComponents;
	SS::HashMap<SObjHashCode, SGameObject*> _TransformCommitNeededObjs;

	IRenderWorld* _RenderWorld = nullptr;

public:
	SWorld();
	virtual ~SWorld();
	void PostConstruct() override;
	void PreDestruct() override;
	virtual void InitializeWorld(IRenderWorld* InRenderWorld);

public:
	void PerFrame();

	IRenderWorld* GetRenderWorld() const { return _RenderWorld; }
	SGameObject* GetWorldRootObject() const { return _WorldRootObject; }
	bool IsAnyObjectRemainInWorld() const;

	void AddToWorld(SGameObject* InNewWorldObject, SGameObject* ParentObject = nullptr);
	void RemoveFromWorld(SGameObject* InObjectToRemove);
	void DestroyAllObjectsInWorld();

	void ProcessTransformCommit();
	void AddTransformCommitNeededObj(SGameObject* InObj);

private:
	void AddToWorld_Recursion(SGameObject* InNewObject);
	void RemoveFromWorld_Recursion(SGameObject* InObjectToRemove);

	void AddGameObjectItem(SGameObject* InNewObject);
	void RemoveGameObjectItem(SGameObject* InObjectToRemove);

private:
	void AddWorldRootObject(SGameObject* InWorldRootObject);
};
