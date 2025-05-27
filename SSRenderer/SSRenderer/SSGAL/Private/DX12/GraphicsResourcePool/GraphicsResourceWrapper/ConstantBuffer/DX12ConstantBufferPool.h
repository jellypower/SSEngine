#pragma once
#include <d3d12.h>

#include "SSGAL/Public/GraphicsResourcePool/GraphicsResourceWrapper/ConstantBuffer/ConstantBufferPool.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


struct DX12ConstantBufferWrapper;

class DX12ConstantBufferPool : public ConstantBufferPool
{
public:
	DX12ConstantBufferPool(const ConstantBufferPoolDesc& Desc);
	virtual ~DX12ConstantBufferPool();

	virtual bool IsValid() const override;
	ConstantBufferWrapper* RentConstantBuffer() override;
	void ResetAllocState() override;

private:
	void IncreaseAllocOffset();
	HRESULT IncreaseBufferPage();
	void CleanUpResources();

private:
	SS::PooledList<ID3D12Resource*> _constantBufferResourcePages;
	SS::PooledList<byte*> _constantBufferResourceSysmemPages;
	SS::PooledList<ID3D12DescriptorHeap*> _descriptorHeaps;
	SS::PooledList<DX12ConstantBufferWrapper*> _entityPool;

	int32 _curAllocOffsetPage = 0;
	int32 _curAllocEntityOffset = 0;
};
