#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/GALPCommonShaderPool.h"

class DX12GALShaderPool : public GALPCommonShaderPool
{
public:
	GALShaderWrapper* CreateShader(const ShaderConstructDesc& InDesc) override;
};
