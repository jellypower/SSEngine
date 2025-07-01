#pragma once
#include "SObject/Public/SObjHashCode.h"

class BasicRenderInstance;
enum class ERenderInstanceType;

// GAL Render Instance Metadata
class GALRIMetadata : INoncopyable
{
protected:
	SObjHashCode _OwnerHashCode;
	const BasicRenderInstance* _OwnerRenderInstance = nullptr;

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() = 0;

};

