#pragma once

class GALRenderDevice;
class IMeshAsset;


class GALMeshAssetWrapperBase : public ISSUnknown
{
protected:
	IMeshAsset* _OwnerMeshAsset = nullptr;
	
};

