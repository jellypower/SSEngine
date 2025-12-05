#pragma once
#include "SRenderComponentBase.h"

class SMeshRenderComponentBase : public SRenderComponentBase
{
protected:
	SS::SHasherW _ModelAssetName;

public:
	void SetModelAsset(SS::SHasherW ModelAssetName);

};

