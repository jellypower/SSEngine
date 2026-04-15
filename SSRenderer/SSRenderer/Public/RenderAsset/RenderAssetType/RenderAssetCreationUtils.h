#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSRenderer/ModuleExportKeyword.h"

enum class ETextureType : int32;
class ITextureAssetMutable;
class IMeshAssetMutable;
class IModelAssetMutable;
class IModelCombinationAssetMutable;
class IMaterialAssetMutable;
class IRenderAnimAssetMutable;


SSRENDERER_MODULE_NATIVE ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType, time_t LastUpdateTime = 0);
SSRENDERER_MODULE_NATIVE IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime = 0);
SSRENDERER_MODULE_NATIVE IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime = 0);
SSRENDERER_MODULE_NATIVE IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt, time_t LastUpdateTime = 0);
SSRENDERER_MODULE_NATIVE IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime = 0);
SSRENDERER_MODULE_NATIVE IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, time_t LastUpdateTime = 0);