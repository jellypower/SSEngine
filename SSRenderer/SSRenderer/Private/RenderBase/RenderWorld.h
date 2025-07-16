#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSRenderer/Public/RenderBase/IRenderWorld.h"

class IRenderer;
class IRenderInstance;

constexpr int32 RENDERWORLD_HASHMAP_SIZE = 1024 * 16;
constexpr int32 RENDERWORLD_BUCKET_CAPACITY = 1024;

class RenderWorld : public IRenderWorld
{
public:
	RenderWorld(const utf16* InWorldName);
	~RenderWorld();
	void InitializeRenderWorld(IRenderer* OwnerRenderer);

public:
	const SS::HashMap<SObjHashCode, IRenderInstance*>& GetRenderInstanceMap() const { return _RenderInstanceByHashCode; }

	virtual bool IsAnyInstanceRemainInWorld() const override;
	virtual SS::SHasherW GetWorldName() const override;
	
	void AddToWorld(IRenderInstance* InRenderInstance) override;
	void RemoveFromWorld(SObjHashCode RenderInstanceIDToRemove) override;

	virtual GALRWMetaData* GetGALMetadata() const override;
	virtual void InjectGALMetadataXXX(GALRWMetaData* InMetadata) override;


private:
	SS::SHasherW _RenderWorldName;

	GALRWMetaData* _GALMetadata = nullptr;

	IRenderer* _OwnerRenderer = nullptr;
	SS::HashMap<SObjHashCode, IRenderInstance*> _RenderInstanceByHashCode;
};
