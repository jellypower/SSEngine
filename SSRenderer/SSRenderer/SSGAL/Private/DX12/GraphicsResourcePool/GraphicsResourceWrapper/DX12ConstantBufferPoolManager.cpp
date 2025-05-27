#include "DX12ConstantBufferPoolManager.h"

#include "ConstantBuffer/DX12ConstantBufferPool.h"


constexpr int32 DX12_CBITEM_SIZE_MIN = 256;

void DX12ConstantBufferPoolManager::InstantiateConstantBufferPool(const ConstantBufferPoolDesc& Desc)
{
	ConstantBufferPool** ppConstantBufferPool= _CBMapByItemSize.Find(Desc.BufferItemSize);
	if (ppConstantBufferPool != nullptr)
	{
		DEBUG_BREAK("Already size of %d Constant Buffer Pool Exists.", Desc.BufferItemSize);
		return;
	}

	if (Desc.BufferItemSize < DX12_CBITEM_SIZE_MIN)
	{
		SS_ASSERT_MSG(false, "sizeof(%d) constant buffer item size is invalid.", Desc.BufferItemSize);
		return;
	}

	DX12ConstantBufferPool* newConstantBufferPool = DBG_NEW DX12ConstantBufferPool(Desc);
	_CBMapByItemSize.Add(Desc.BufferItemSize, newConstantBufferPool);
}
