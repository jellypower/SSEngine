#include "DX12GPUMeshAssetInstance.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"


DX12GPUMeshAssetInstance::DX12GPUMeshAssetInstance(MeshAsset* ownerMeshAsset, DX12GALRenderDevice* InOwnerRenderDevice)
{
	_OwnerMeshAsset = ownerMeshAsset;
	_OwnerRenderDevice = InOwnerRenderDevice;
}

DX12GPUMeshAssetInstance::~DX12GPUMeshAssetInstance()
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
