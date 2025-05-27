#include "MaterialAsset.h"

MaterialAsset::MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) :
	SSAssetBase(EAssetType::Material, InAssetName, InAssetPath)
{

}
