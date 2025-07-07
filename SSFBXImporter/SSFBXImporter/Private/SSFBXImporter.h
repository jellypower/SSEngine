#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

#define FBXSDK_SHARED
#include <fbxsdk.h>

#include "SSFBXImporter/Public/ISSFBXImporter.h"

class IModelCombinationAssetMutable;
class IRenderer;
struct AssetPlacementReference;
class MaterialAsset;
class MeshAsset;
class SSAssetBase;


class SSFBXImporter : public ISSFBXImporter
{
private:
	FbxManager* _FBXManager = nullptr;
	FbxImporter* _FBXImporter = nullptr;
	FbxScene* _currentScene = nullptr;
	SS::SHasherW _boundFilePath;
	SS::SHasherW _boundFileName;

	SS::PooledList<SS::pair<::FbxMesh*, SS::SHasherW>> _importedMeshNames;

	IAssetManagerMutable* _AssetManagerToImportAsset = nullptr;

public:
	SSFBXImporter();
	virtual ~SSFBXImporter();

public:
	virtual SS::SHasherW GetBoundFilePath() const override;
	virtual SS::SHasherW GetBoundFileName() const override;

	virtual bool BindFbxSceneFile(const utf16* inFilePath) override;
	virtual void ClearFbxSceneFile() override;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager) override;
	virtual void ClearRendererToImportAsset() override;

	virtual void ImportCurrentFileToAssetManager() override;

private:
	void ImportCurrentFileToMaterialAsset();
	void ImportCurrentFileToModelAsset();

	void ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, IModelCombinationAssetMutable* MdlcAsset);

	void PrintFbxNodeInfo(FbxNode* node);

	void ImportCurrentFileToAnimAsset();

};