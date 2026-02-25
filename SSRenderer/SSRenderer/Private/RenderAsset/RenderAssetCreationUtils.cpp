#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCreationUtils.h"


#include "RenderAssetType/MaterialAsset.h"
#include "RenderAssetType/MeshAsset.h"
#include "RenderAssetType/ModelAsset.h"
#include "RenderAssetType/ModelCombinationAsset.h"
#include "RenderAssetType/RenderAnimAsset.h"
#include "RenderAssetType/TextureAsset.h"

ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, ETextureType InType)
{
	return DBG_NEW TextureAsset(InDBNameSpace, InAssetName, InAssetPath, InType);
}

IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	return DBG_NEW MeshAsset(InDBNameSpace, InAssetName, InAssetPath);
}

IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath)
{
	return DBG_NEW ModelAsset(InDBNameSpace, InAssetName, InAssetPath);
}

IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, int32 ReservedChildCnt)
{
	return DBG_NEW ModelCombinationAsset(InDBNameSpace, InAssetName, InAssetPath, ReservedChildCnt);
}

IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath)
{
	return DBG_NEW MaterialAsset(InDBNameSpace, InAssetName, InAssetPath);
}

IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath)
{
	return DBG_NEW RenderAnimAsset(InDBNameSpace, InAssetName, InAssetPath);
}