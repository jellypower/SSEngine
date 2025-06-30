#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"


class ShaderAsset;

class GPUShaderAssetInstanceBase : public INoncopyable
{
protected:
	ShaderAsset* _OwnerShaderAsset;

public:
	virtual bool IsValid() const = 0;
};
