#pragma once
#include "SSGAL/Public/GraphicsResourcePool/ConstantBufferPoolManager.h"

class DX12ConstantBufferPoolManager : public ConstantBufferPoolManager
{
public:
	void InstantiateConstantBufferPool(const ConstantBufferPoolDesc& Desc) override;
};
