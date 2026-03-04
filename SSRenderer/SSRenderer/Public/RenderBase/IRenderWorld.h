#pragma once
#include "SObject/Public/SObjHashCode.h"

class IRenderLight;
class IRenderInstance;
class GALRWMetaData;
class IRenderer;

class IRenderWorld : public INoncopyable
{
public:
	virtual bool IsAnyInstanceRemainInWorld() const = 0;
	virtual SS::SHasherW GetWorldName() const = 0;

	virtual void AddToWorld(IRenderInstance* InRenderInstance) = 0;

	virtual void RemoveRenderInstanceFromWorld(SObjHashCode RenderInstanceIDToRemove) = 0;

	virtual GALRWMetaData* GetGALMetadata(int32 FrameMod) const = 0;
	virtual void InjectGALMetadataXXX(GALRWMetaData* InMetadata, int32 FrameMod) = 0;
};
