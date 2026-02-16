#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"

class IRenderLight;
class IRenderInstance;

constexpr int32 RENDERWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 RENDERWORLD_BUCKET_CAPACITY = 1024;

constexpr int32 RENDERLIGHT_HASHMAP_SIZE = 100;
constexpr int32 RENDERLIHGT_BUCKET_CAPACITY = 10;

class RenderWorld : public IRenderWorld
{
public:
	RenderWorld(const utf16* InWorldName);
	virtual ~RenderWorld();

public:
	const SS::HashMap<SObjHashCode, IRenderInstance*>& GetRenderInstanceMap() const { return _RenderInstanceByHashCode; }

	virtual bool IsAnyInstanceRemainInWorld() const override;
	virtual SS::SHasherW GetWorldName() const override;
	
	void AddToWorld(IRenderInstance* InRenderInstance) override;
	virtual void RemoveRenderInstanceFromWorld(SObjHashCode RenderInstanceIDToRemove) override;

	virtual GALRWMetaData* GetGALMetadata() const override;
	virtual void InjectGALMetadataXXX(GALRWMetaData* InMetadata) override;


private:
	SS::SHasherW _RenderWorldName;

	GALRWMetaData* _GALMetadata = nullptr;

	SS::HashMap<SObjHashCode, IRenderInstance*> _RenderInstanceByHashCode;
};
