#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IModelCombinationAsset;
class SGameObject;

class SRendererUtil
{
public:
	static SGameObject* InstantiateModelObjTree(SS::SHasherW MdlcAssetName);
	static SGameObject* InstantiateModel(SS::SHasherW ModelAssetName);

private:
	static void InstantiateModelObjTree_Recursion(const IModelCombinationAsset* MdlcAsset, int32 CurAssetIdx, SGameObject* CurGameObject);
};
