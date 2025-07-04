#pragma once
#include "IAssetBase.h"

class GALMaterialAssetWrapperBase;

class IMaterialAsset : public IAssetBase
{
public:
	GALMaterialAssetWrapperBase* _GALMaterial = nullptr;

	// TEMP
public:
	SS::SHasherW _PSName;
	// ~TEMP

};
