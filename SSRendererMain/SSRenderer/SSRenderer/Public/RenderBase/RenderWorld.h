#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class SSRenderer;
class IRenderInstance;

constexpr int32 RENDERWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 RENDERWORLD_BUCKET_CAPACITY = 1024;

class RenderWorld : public INoncopyable
{
public:
	RenderWorld();
	void InitializeRenderWorld(SSRenderer* OwnerRenderer);

public:
	const SS::HashMap<SObjHashCode, IRenderInstance*>& GetRenderInstanceMap() const { return _RenderInstanceByHashCode; }

	bool IsAnyInstanceRemainInWorld() const;
	
	void AddToWorld(IRenderInstance* InRenderInstance);
	void RemoveFromWorld(SObjHashCode RenderInstanceIDToRemove);

	void CommitRenderInstanceTransform();


private:
	SSRenderer* _OwnerRenderer;

	SS::HashMap<SObjHashCode, IRenderInstance*> _RenderInstanceByHashCode;
};
