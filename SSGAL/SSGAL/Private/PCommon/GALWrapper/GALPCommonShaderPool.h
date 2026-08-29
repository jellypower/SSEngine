#pragma once
#include "SSGAL/Public/GALWrapper/GALShaderPool.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"



class GALPCommonShaderPool : public GALShaderPool
{
protected:
	SS::HashMap<SS::SHasherW, GALShaderWrapper*> _ShaderMap;

public:
	GALPCommonShaderPool();


	virtual void Initialize() override;
	virtual GALShaderWrapper* FindShader(SS::SHasherW InShaderName) const override;
};
