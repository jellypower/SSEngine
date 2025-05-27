#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

struct ConstantBufferWrapper;
struct ConstantBufferPoolDesc;
class ConstantBufferPool;

class ConstantBufferPoolManager : public INoncopyable
{
public:
	ConstantBufferPoolManager();
	virtual ~ConstantBufferPoolManager();

	virtual void InstantiateConstantBufferPool(const ConstantBufferPoolDesc& Desc) = 0;
	virtual void ReleaseAllConstantBufferPool();

	ConstantBufferWrapper* RentConstantBuffer(int32 BufferSize);


	void ResetAllAllocatedConstantBuffers();
protected:
	SS::HashMap<int32, ConstantBufferPool*> _CBMapByItemSize;
};