#pragma once
// #include <d3dx12.h>

#include "SSGAL/Private/PCommon/GALWrapper/PSOWrapper.h"

class DX12PSOWrapper : public PSOWrapper
{
private:
	const static D3D12_INPUT_ELEMENT_DESC* GetInputElementDesc(EInputLayoutType InputElementType, uint32& outElementCnt);

private:
	ID3D12PipelineState* _PipelineState = nullptr;
	
	
public:
	DX12PSOWrapper(const PipelineDesc& pipelineDesc, PSOPool* InOwnerPSOPool);
	virtual ~DX12PSOWrapper();

	virtual bool IsValid() const override;

public:
	ID3D12PipelineState* GetPipelineState() const { return _PipelineState; }
};
