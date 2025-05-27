#pragma once
#include "SSAssetBase.h"

class GPUMaterialAssetInstanceBase;

class MaterialAsset : public SSAssetBase
{
public:
	GPUMaterialAssetInstanceBase* _GPUInstance = nullptr;

	// TEMP
public:
	SS::SHasherW _VSName;
	SS::SHasherW _PSName;


	// ~TEMP

public:
	MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath);

};