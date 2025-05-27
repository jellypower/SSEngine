#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class SSRenderer;
class BasicRenderInstance;

constexpr int32 RENDERWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 RENDERWORLD_BUCKET_CAPACITY = 1024;

class RenderWorld : public INoncopyable
{
public:
	RenderWorld();
	void InitializeRenderWorld(SSRenderer* OwnerRenderer);

public:
	const SS::HashMap<SObjHashCode, BasicRenderInstance*>& GetRenderInstanceMap() const { return _RenderInstanceByHashCode; }

	bool IsAnyInstanceRemainInWorld() const;
	
	void AddToWorld(BasicRenderInstance* RenderInstance);
	void RemoveFromWorld(SObjHashCode RenderInstanceToRemove);

	void CommitRenderInstanceTransform();


private:
	SSRenderer* _OwnerRenderer;

	SS::HashMap<SObjHashCode, BasicRenderInstance*> _RenderInstanceByHashCode;
};
