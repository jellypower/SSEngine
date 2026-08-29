#include "pch.h"


#include "DX12GALShaderPool.h"

#include "DX12GALShaderWrapper.h"

void DX12GALShaderPool::Release()
{
	for (SS::pair<SS::SHasherW, GALShaderWrapper*>& Item : _ShaderMap)
	{
		GALShaderWrapper* ShaderItem = Item.second;
		ShaderItem->ReleaseShaderInstance();
		ShaderItem->Release();
	}
}

GALShaderWrapper* DX12GALShaderPool::CreateShader(const ShaderConstructDesc& InDesc)
{
	DX12GALShaderWrapper* NewShaderWrapper = DBG_NEW DX12GALShaderWrapper(InDesc, this);
	return NewShaderWrapper;
}
