#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/GALPCommonShaderPool.h"

class DX12GALShaderPool : public GALPCommonShaderPool
{
public:
	void Release() override;

public:
	GALShaderWrapper* CreateShader(const ShaderConstructDesc& InDesc) override;

};
