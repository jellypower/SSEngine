#include "DX12RootSignaturePool.h"

#include "GraphicsResourceWrapper/DX12RootSignatureWrapper.h"

void DX12RootSignaturePool::InstantiateAllRootSignatures()
{
	for (int32 i = 0; i < (int32)ERootSignatureType::COUNT; i++)
	{
		_rootSignatures[i] = DBG_NEW DX12RootSignatureWrapper(static_cast<ERootSignatureType>(i));
		if (_rootSignatures[i]->IsValid() == false)
		{
			delete _rootSignatures[i];
			_rootSignatures[i] = nullptr;
		}
	}
}

void DX12RootSignaturePool::ReleaseAllRoogSignatures()
{
	for (int32 i = 0; i < (int32)ERootSignatureType::COUNT; i++)
	{
		if(_rootSignatures[i]->IsValid())
		{
			delete _rootSignatures[i];
			_rootSignatures[i] = nullptr;
		}
	}
}
