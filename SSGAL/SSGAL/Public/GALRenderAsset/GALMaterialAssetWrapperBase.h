#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IMaterialAsset;
class GALRenderDevice;

class GALMaterialAssetWrapperBase : public INoncopyable
{
protected:
	IMaterialAsset* _OwnerMaterialAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};
