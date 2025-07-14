#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class IMaterialAsset;
class GALRenderDevice;

class GALMaterialAssetWrapperBase : public INoncopyable // TODO: 여기서 계속하기(25/07/12)
{
protected:
	IMaterialAsset* _OwnerMaterialAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};
