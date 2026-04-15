#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IMaterialAsset;
class GALRenderDevice;

class GALMaterialAssetWrapperBase : public INoncopyable
{
public:
	virtual void SyncMtlParam() = 0;

protected:
	IMaterialAsset* _OwnerMaterialAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};
