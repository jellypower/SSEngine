#include "DX12GPUMeshAssetInstance.h"

#include "SSEngineDefault/Public/SSDebugLogger.h"


DX12GPUMeshAssetInstance::DX12GPUMeshAssetInstance(MeshAsset* ownerMeshAsset)
	: GPUMeshAssetInstanceBase(ownerMeshAsset)
{
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

bool DX12GPUMeshAssetInstance::IsValid() const
{
	return _VertexBuffer != nullptr;
}
