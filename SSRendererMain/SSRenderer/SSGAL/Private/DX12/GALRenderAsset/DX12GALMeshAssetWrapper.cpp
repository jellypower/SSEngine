#include "DX12GALMeshAssetWrapper.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"


DX12GALMeshAssetWrapper::DX12GALMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerMeshAsset = ownerMeshAsset;
	_OwnerRenderDevice = InOwnerRenderDevice;
}

DX12GALMeshAssetWrapper::~DX12GALMeshAssetWrapper()
{
	if (_VertexBuffer != nullptr)
	{
		_VertexBuffer->Release();
	}

	if (_IndexBuffer != nullptr)
	{
		_IndexBuffer->Release();
	}
}
