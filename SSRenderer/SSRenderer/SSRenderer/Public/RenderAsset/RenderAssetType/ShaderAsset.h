#pragma once
#include "SSAssetBase.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSRenderer/Public/SSRendererBuildSetings.h"


class GALShaderWrapper;




class ShaderAsset : public SSAssetBase
{
public:
	ShaderAsset(SS::SHasherW InAssetName);

	const GALShaderWrapper* GetShaderWrapper() const { return _GALShaderWrapperCache;  }



public:
	GALShaderWrapper* _GALShaderWrapperCache = nullptr;

};