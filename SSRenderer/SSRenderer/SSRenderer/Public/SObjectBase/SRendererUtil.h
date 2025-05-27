#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class ModelCombinationAsset;
class SGameObject;

class SRendererUtil
{
public:
	static SGameObject* InstantiateModelObjTree(SS::SHasherW MdlcAssetName);
	static SGameObject* InstantiateModel(SS::SHasherW ModelAssetName);

private:
	static void InstantiateModelObjTree_Recursion(const ModelCombinationAsset* MdlcAsset, int32 CurAssetIdx, SGameObject* CurGameObject);
};
