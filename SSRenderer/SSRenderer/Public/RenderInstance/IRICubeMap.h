#pragma once
#include "IRenderInstance.h"

class ITextureAsset;

class IRICubeMap : public IRenderInstance
{
public:
	virtual float GetCubeMapSize() const = 0;
	virtual ITextureAsset* GetCubemapTexture() const = 0;
	virtual void SetCubemapTexture(ITextureAsset* InAsset) = 0;
};
