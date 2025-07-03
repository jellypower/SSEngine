#pragma once
#include "IAssetBase.h"
#include "RenderAssetCommon/CommonDataType.h"


class SSAssetBase : public IAssetBase
{
public:
	SSAssetBase(EAssetType InAssetType, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
	SSAssetBase(EAssetType InAssetType);
	virtual ~SSAssetBase();

public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) override;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) override;


};

