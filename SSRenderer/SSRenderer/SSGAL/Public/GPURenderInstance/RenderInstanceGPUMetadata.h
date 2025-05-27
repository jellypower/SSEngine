#pragma once
#include "SObject/Public/SObjHashCode.h"

class BasicRenderInstance;
enum class ERenderInstanceType;

class RenderInstanceGPUMetadata : INoncopyable
{
protected:
	SObjHashCode _OwnerHashCode;
	const BasicRenderInstance* _OwnerRenderInstance = nullptr;

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() = 0;

};

