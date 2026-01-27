#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;

class ISSFBXImporter : public INoncopyable
{
public:
	virtual SS::SHasherW GetBoundFilePath() const = 0;
	virtual SS::SHasherW GetBoundFileName() const = 0;
	virtual SS::PooledList<IAssetBase*> GetImportedAssets() const = 0;


public:
	virtual bool BindFbxSceneFile(const utf16* inFilePath) = 0;
	virtual void ClearFbxSceneFile() = 0;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager, ICommonRenderAssetSet* InCommonRenderAssetSet) = 0;
	virtual void ClearRendererToImportAsset() = 0;

	virtual void RelocateImportedAssetsToAssetManager() = 0;

	virtual void GenerateImportedAssets() = 0;

};
