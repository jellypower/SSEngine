#include "pch.h"


#include "DX12GALShaderPool.h"

#include "DX12GALShaderWrapper.h"

GALShaderWrapper* DX12GALShaderPool::CreateShader(const ShaderConstructDesc& InDesc)
{
	DX12GALShaderWrapper* NewShaderWrapper = DBG_NEW DX12GALShaderWrapper(InDesc, this);
	return NewShaderWrapper;
}
