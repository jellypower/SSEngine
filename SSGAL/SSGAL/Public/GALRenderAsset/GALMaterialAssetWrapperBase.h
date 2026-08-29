#pragma once

class IMaterialAsset;
class GALRenderDevice;

class GALMaterialAssetWrapperBase : public ISSUnknown
{
public:
	virtual void SyncMtlParam() = 0;

protected:
	IMaterialAsset* _OwnerMaterialAsset = nullptr;
	GALRenderDevice* _OwnerRenderDevice = nullptr;
};
