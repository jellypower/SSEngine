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


SSRENDERER_MODULE ITextureAssetMutable* CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType);
SSRENDERER_MODULE IMeshAssetMutable* CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
SSRENDERER_MODULE IModelAssetMutable* CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
SSRENDERER_MODULE IModelCombinationAssetMutable* CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, int32 ReservedChildCnt);
SSRENDERER_MODULE IMaterialAssetMutable* CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
SSRENDERER_MODULE IRenderAnimAssetMutable* CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);