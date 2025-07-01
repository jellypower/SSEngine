#pragma once
#include "SSAssetBase.h"

class GALMaterialAssetWrapperBase;

class MaterialAsset : public SSAssetBase
{
public:
	GALMaterialAssetWrapperBase* _GALMaterial = nullptr;

	// TEMP
public:
	SS::SHasherW _VSName;
	SS::SHasherW _PSName;


	// ~TEMP

public:
	MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

};