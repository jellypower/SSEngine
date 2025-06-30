#include "ShaderAsset.h"



ShaderAsset::ShaderAsset(SS::SHasherW InAssetName):
	SSAssetBase(EAssetType::Shader, InAssetName, SS::SHasherW::Empty)
{
}

