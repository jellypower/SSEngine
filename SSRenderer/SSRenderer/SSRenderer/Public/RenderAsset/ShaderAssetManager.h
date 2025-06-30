#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class SSRenderer;
class ShaderAsset;

class ShaderAssetManager : public INoncopyable
{
public:
	ShaderAssetManager(SSRenderer* InOwnerRenderer);
	virtual ~ShaderAssetManager();

	virtual ShaderAsset* FindShaderAsset(SS::SHasherW InShaderName) const;

	void InstantiateAllShaders();
	void ReleaseAllShaders();

protected:
	SSRenderer* _OwnerRenderer = nullptr;

	SS::HashMap<SS::SHasherW, ShaderAsset*> _shaderMap;
};

