#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSContentsBase/ModuleExportKeyword.h"

class IModelCombinationAsset;
class SGameObject;

class SSCONTENTBASE_MODULE SRendererUtil
{
public:
	static SGameObject* InstantiateMDLC(SS::SHasherW MdlcAssetName, bool bFinishConstruct = true);
	static SGameObject* InstantiateModel(SS::SHasherW ModelAssetName, SS::SHasherW ObjectNameOverride = SS::SHasherW::GetEmpty(), bool bFinishConstruct = true);
	static SGameObject* InstantiateMesh(SS::SHasherW MeshAssetName, SS::SHasherW ObjectNameOverride = SS::SHasherW::GetEmpty(), bool bFinishConstruct = true);

};
