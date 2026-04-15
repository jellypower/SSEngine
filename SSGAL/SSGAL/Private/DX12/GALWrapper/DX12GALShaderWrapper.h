#pragma once
// #include <d3d12.h>

#include "SSGAL/Public/GALWrapper/GALShaderWrapper.h"

class DX12GALShaderWrapper : public GALShaderWrapper
{
public:
	DX12GALShaderWrapper(const ShaderConstructDesc& InDesc, GALShaderPool* InOwnerPool);

	virtual bool CompileShaderInstance() override;
	virtual void ReleaseShaderInstance() override;


	ID3DBlob* GetCompiledShader() const { return _compiledShader; }

private:
	ID3DBlob* _compiledShader = nullptr;
};
