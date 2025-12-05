#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class IModelCombinationAsset;
class SGameObject;

class SSCONTENTBASE_MODULE SRendererUtil
{
public:
	static SGameObject* InstantiateModelObjTree(SS::SHasherW MdlcAssetName);
	static SGameObject* InstantiateModel(SS::SHasherW ModelAssetName, SS::SHasherW ObjectNameOverride = SS::SHasherW::GetEmpty());

private:
	static void InstantiateModelObjTree_Recursion(const IModelCombinationAsset* MdlcAsset, int32 CurAssetIdx, SGameObject* CurGameObject, SGameObject* StrongBindAncestor);
};
