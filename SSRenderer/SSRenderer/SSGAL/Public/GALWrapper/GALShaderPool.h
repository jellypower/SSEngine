#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

struct ShaderConstructDesc;
class GALShaderWrapper;

class GALShaderPool : public INoncopyable
{
public:
	virtual void Initialize() = 0;

	virtual GALShaderWrapper* FindShader(SS::SHasherW InShaderName) const = 0;

protected:
	virtual GALShaderWrapper* CreateShader(const ShaderConstructDesc& InDesc) = 0;
};
