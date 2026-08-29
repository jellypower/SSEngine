#pragma once
#include <SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h>

#include "DX12GALMeshAssetWrapper.h"

class DX12GALSkinnedMeshAssetWrapper : public DX12GALMeshAssetWrapper
{
public:
	DX12GALSkinnedMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDeviceContext* ResourceUpdateExecutor);
	void Release() override;

public:
	ID3D12Resource* _OriginalJointInverseResource = nullptr;
	AllocatedChunkHeader _OriginalJointInverseSRVDescriptorChunk;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _OriginalJointInverseSRVDescHandle;
};
