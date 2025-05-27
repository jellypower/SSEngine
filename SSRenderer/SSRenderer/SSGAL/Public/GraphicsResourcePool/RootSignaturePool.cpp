#include "RootSignaturePool.h"

RootSignaturePool::~RootSignaturePool()
{
}

const RootSignatureWrapper* RootSignaturePool::GetRootSignature(ERootSignatureType InType) const
{
	return _rootSignatures[(int32)InType];
}
