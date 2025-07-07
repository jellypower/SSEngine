#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class IAssetManagerMutable;

class ISSFBXImporter : public INoncopyable
{
public:
	virtual SS::SHasherW GetBoundFilePath() const = 0;
	virtual SS::SHasherW GetBoundFileName() const = 0;

public:
	virtual bool BindFbxSceneFile(const utf16* inFilePath) = 0;
	virtual void ClearFbxSceneFile() = 0;

	virtual void BindAssetManagerToImportAsset(IAssetManagerMutable* InAssetMnanager) = 0;
	virtual void ClearRendererToImportAsset() = 0;

	virtual void ImportCurrentFileToAssetManager() = 0;
};
