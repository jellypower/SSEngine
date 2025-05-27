#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
class SSAssetBase;


class GPUAssetInstanceBase : public INoncopyable
{
public:
	virtual bool IsValid() const = 0;
protected:
	SSAssetBase* _ownerAsset = nullptr;

};
