#include "pch.h"


#include "DX12RootSignaturePool.h"

#include "DX12RootSignatureWrapper.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

DX12RootSignaturePool::DX12RootSignaturePool(DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerRenderDevice = InOwnerRenderDevice;
}

void DX12RootSignaturePool::InstantiateAllRootSignatures()
{
	for (int32 i = 0; i < (int32)ERootSignatureType::COUNT; i++)
	{
		_rootSignatures[i] = DBG_NEW DX12RootSignatureWrapper(static_cast<ERootSignatureType>(i), this);
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
