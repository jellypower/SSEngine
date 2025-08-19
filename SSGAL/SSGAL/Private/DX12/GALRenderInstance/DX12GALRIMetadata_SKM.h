#pragma once
#include "DX12GALRIMetadata_SM.h"

struct SBASkinningJointMatrix;
class IRISkinnedMesh;



class DX12GALRIMetadata_SKM : public DX12GALRIMetadata_SM
{
public:
	DX12GALRIMetadata_SKM(GALRenderDevice* InRenderDevice, const IRISkinnedMesh* InOwnerRenderInstance);
	virtual ~DX12GALRIMetadata_SKM();
public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;

public:
	ID3D12Resource* _JointSBResource = nullptr;

	AllocatedChunkHeader _JointSRVDescTableChunk;
	ID3D12DescriptorHeap* _CachedJointSRVDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _JointSRVDescTableCPU = {};
	CD3DX12_GPU_DESCRIPTOR_HANDLE _JointSRVDescTableGPU = {};
};
