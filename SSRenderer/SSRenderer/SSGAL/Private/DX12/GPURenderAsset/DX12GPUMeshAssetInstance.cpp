#include "DX12GPUMeshAssetInstance.h"


DX12GPUMeshAssetInstance::DX12GPUMeshAssetInstance(MeshAsset* ownerMeshAsset)
{
	_OwnerMeshAsset = ownerMeshAsset;
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
