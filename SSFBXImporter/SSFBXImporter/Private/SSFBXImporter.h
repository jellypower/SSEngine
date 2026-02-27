#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSFBXImporter/Public/FRAN.h"


#include "SSFBXImporter/Public/ISSFBXImporter.h"

enum class EAssetType;
class IMaterialAsset;
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


private:
	SS::SHasherW _boundFilePath;
	SS::SHasherW _boundFileName;
	SS::SHasherW _RepresentingAssetName;


private:
	SS::PooledList<SS::pair<::FbxMesh*, SS::SHasherW>> _importedMeshNames;
	SS::HashMap<uint64, IMaterialAsset*> _FbxUniqueIDToMtlAsset;
	SS::PooledList<IAssetBase*> _ImportedAssets;
	SS::PooledList<SS::SHasherW> _IssuedAssetNamesForThisBind;


public:
	SSFBXImporter();
	virtual ~SSFBXImporter();

public:
	virtual SS::SHasherW GetBoundFilePath() const override;
	virtual SS::SHasherW GetBoundFileName() const override;
	virtual SS::SHasherW GetRepresentingAssetName() const override;

	virtual SS::PooledList<IAssetBase*> GetImportedAssets() const override;

	virtual bool BindFbxSceneFile(const utf16* inFilePath) override;
	virtual void ClearFbxSceneFile() override;

	virtual void RelocateCreatedAssets(SS::PooledList<IAssetBase*>& OutAssetList) override;

	virtual void GenerateImportedAssets() override;

private:
	IAssetBase* FindImportedAssetByName(SS::SHasherW InAssetName, EAssetType InAssetType) const;

	template<typename TAsset>
	TAsset* FindImportedAssetByName(SS::SHasherW InAssetName) const
	{
		static_assert(std::derived_from<TAsset, IAssetBase>);
		return static_cast<TAsset*>(FindImportedAssetByName(InAssetName, TAsset::ThisAssetType));
	}

private:
	SS::SHasherW IssueNewAssetName(const SS::StringW& nodeName, EAssetType InAssetType);

	void GenerateImportedMaterialAssets();
	void GenerateImportedMdlcAsset();

	void ImportCurrentFileToModelAsset_Recursion(::FbxNode* node, int32 parentReferenceIdx, IModelCombinationAssetMutable* MdlcAsset);

	void GenerateImportedRenderAnimAssets();
};