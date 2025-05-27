#include "ConstantBufferPoolManager.h"

#include "GraphicsResourceWrapper/ConstantBuffer/ConstantBufferPool.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

ConstantBufferPoolManager::ConstantBufferPoolManager()
	: _CBMapByItemSize(CONSTANTBUFFER_DEFAULT_HASHMAP_CAPACITY, 10)
{
}

ConstantBufferPoolManager::~ConstantBufferPoolManager()
{
}

void ConstantBufferPoolManager::ReleaseAllConstantBufferPool()
{
	for(const SS::pair<int32, ConstantBufferPool*>& constantBufferPairItem : _CBMapByItemSize)
	{
		delete constantBufferPairItem.second;
	}

	_CBMapByItemSize.Clear();
}

ConstantBufferWrapper* ConstantBufferPoolManager::RentConstantBuffer(int32 BufferSize)
{
	ConstantBufferPool** ppCBPool = _CBMapByItemSize.Find(BufferSize);

	if (ppCBPool == nullptr)
	{
		SS_ASSERT_MSG(false, L"sizeof(%d) Constant buffer pool is not Initialized", BufferSize);
	}

	ConstantBufferPool* CBPool = *ppCBPool;

	ConstantBufferWrapper* Wrapper = CBPool->RentConstantBuffer();
	
	return Wrapper;
}

void ConstantBufferPoolManager::ResetAllAllocatedConstantBuffers()
{
	for (const SS::pair<int, ConstantBufferPool*>& _CBPairItem : _CBMapByItemSize)
	{
		_CBPairItem.second->ResetAllocState();
	}
}
