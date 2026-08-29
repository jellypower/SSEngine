#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IAssetBase;
class ICommonRenderAssetSet;
class IAssetManagerMutable;

class ISSFBXImporter : public ISSUnknown
{
public:
	virtual SS::SHasherW GetBoundFilePath() const = 0;
	virtual SS::SHasherW GetBoundFileName() const = 0;
	virtual SS::SHasherW GetRepresentingAssetName() const = 0;
	virtual SS::SHasherW GetBoundNameSpace() const = 0;

	virtual const SS::PooledList<IAssetBase*>& GetImportedAssets() const = 0;


public:
	virtual bool BindFbxSceneFile(const utf16* inFilePath, SS::SHasherW NameSpace) = 0;
	virtual void ClearFbxSceneFile() = 0;


	virtual void RelocateCreatedAssets(SS::PooledList<IAssetBase*>& OutAssetList) = 0;

	virtual void GenerateImportedAssets() = 0;

};
