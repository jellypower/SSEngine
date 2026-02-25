#pragma once

struct ShaderConstructDesc;
class GALShaderWrapper;

class GALShaderPool : public INoncopyable
{
public:
	virtual GALShaderWrapper* FindShader(SS::SHasherW InShaderName) const = 0;
	virtual void Initialize() = 0;	


protected:
	virtual GALShaderWrapper* CreateShader(const ShaderConstructDesc& InDesc) = 0;
};
