#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderAssetCreationUtils.h"


#include "RenderAssetType/MaterialAsset.h"
#include "RenderAssetType/MeshAsset.h"
#include "RenderAssetType/ModelAsset.h"
#include "RenderAssetType/ModelCombinationAsset.h"
#include "RenderAssetType/RenderAnimAsset.h"
#include "RenderAssetType/TextureAsset.h"

ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, ETextureType InType, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW TextureAsset(InDBNameSpace, InAssetName, InAssetPath, InType, LastUpdateTime);
}

IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW MeshAsset(InDBNameSpace, InAssetName, InAssetPath, LastUpdateTime);
}

IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW ModelAsset(InDBNameSpace, InAssetName, InAssetPath, LastUpdateTime);
}

IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, int32 ReservedChildCnt, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW ModelCombinationAsset(InDBNameSpace, InAssetName, InAssetPath, ReservedChildCnt, LastUpdateTime);
}

IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW MaterialAsset(InDBNameSpace, InAssetName, InAssetPath, LastUpdateTime);
}

IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, time_t LastUpdateTime)
{
	if (LastUpdateTime == 0)
	{
		time(&LastUpdateTime);
	}

	return DBG_NEW RenderAnimAsset(InDBNameSpace, InAssetName, InAssetPath, LastUpdateTime);
}