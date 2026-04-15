#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"

class GALRenderDevice;
class IMeshAsset;


class GALMeshAssetWrapperBase : public INoncopyable
{
protected:
	IMeshAsset* _OwnerMeshAsset = nullptr;
	
};

