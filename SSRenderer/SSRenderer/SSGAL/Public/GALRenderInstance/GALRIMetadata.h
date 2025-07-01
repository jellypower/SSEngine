#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IRenderInstance;
enum class ERenderInstanceType;

// GAL Render Instance Metadata
class GALRIMetadata : INoncopyable
{
protected:
	const IRenderInstance* _OwnerRenderInstance = nullptr;

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() = 0;

};

