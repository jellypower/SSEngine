#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#define FBXSDK_SHARED
#include <fbxsdk.h>

class SSRenderer;
class ModelCombinationAsset;
struct AssetPlacementReference;
class MaterialAsset;
class MeshAsset;
class SSAssetBase;

class SSFBXImporter
{
private:
	FbxManager* _FBXManager = nullptr;
	FbxImporter* _FBXImporter = nullptr;
	FbxScene* _currentScene = nullptr;
	SS::SHasherW _boundFilePath = SS::SHasherW::GetEmpty();
	SS::SHasherW _boundFileName = SS::SHasherW::GetEmpty();

	SS::PooledList<SS::pair<::FbxMesh*, SS::SHasherW>> _importedMeshNames;

	SSRenderer* _RendererToImportAsset = nullptr;

public:
	SSFBXImporter();
	~SSFBXImporter();

	SS::SHasherW GetBoundFileName() const { return _boundFileName; }

	bool BindFbxSceneFile(const utf16* inFilePath);
	void ClearFbxSceneFile();

	void SetRendererToImportAsset(SSRenderer* RendererToImport);
	void ImportCurrentFileToAssetManager();
private:
	void ImportCurrentFileToMaterialAsset();
	void ImportCurrentFileToModelAsset();

	void ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, ModelCombinationAsset* MdlcAsset);

	void PrintFbxNodeInfo(FbxNode* node);

	void ImportCurrentFileToAnimAsset();

};