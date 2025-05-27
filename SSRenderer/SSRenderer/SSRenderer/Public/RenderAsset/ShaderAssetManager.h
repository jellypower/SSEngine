#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class ShaderAsset;

class ShaderAssetManager : public INoncopyable
{
public:
	ShaderAssetManager();
	virtual ~ShaderAssetManager();

	virtual ShaderAsset* FindShaderAsset(SS::SHasherW InShaderName) const;

	void InstantiateAllShaders();
	void ReleaseAllShaders();

protected:
	SS::HashMap<SS::SHasherW, ShaderAsset*> _shaderMap;
};

