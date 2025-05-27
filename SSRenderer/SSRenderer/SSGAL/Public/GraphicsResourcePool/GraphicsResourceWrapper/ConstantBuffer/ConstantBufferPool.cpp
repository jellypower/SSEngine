#include "ConstantBufferPool.h"

ConstantBufferPool::ConstantBufferPool(const ConstantBufferPoolDesc& Desc)
{
	_constantBufferitemSize = Desc.BufferItemSize;
	_resourceSizePerPage = Desc.ResourceSizePerPage;
}

ConstantBufferPool::~ConstantBufferPool()
{
	// noop
}
