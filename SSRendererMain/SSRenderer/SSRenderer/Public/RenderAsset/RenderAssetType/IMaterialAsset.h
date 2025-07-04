#pragma once
#include "IAssetBase.h"

class GALMaterialAssetWrapperBase;

class IMaterialAsset : public IAssetBase
{
public:
	GALMaterialAssetWrapperBase* _GALMaterial = nullptr;

	// TEMP
public:
	SS::SHasherW _VSName; // TODO: ¾ø¾Ö±â
	SS::SHasherW _PSName;
	// ~TEMP

};
