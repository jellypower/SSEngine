#pragma once
#include "IAssetBase.h"

class GALMaterialAssetWrapperBase;

class IMaterialAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Material;

public:
	GALMaterialAssetWrapperBase* _GALMaterial = nullptr;

	// TEMP
public:
	SS::SHasherW _PSName;
	// ~TEMP

};
