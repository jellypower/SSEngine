#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class GALRenderDevice;
class MaterialAsset;

class GALMaterialAssetWrapperBase : public INoncopyable
{
protected:
	MaterialAsset* _OwnerMaterialAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};
