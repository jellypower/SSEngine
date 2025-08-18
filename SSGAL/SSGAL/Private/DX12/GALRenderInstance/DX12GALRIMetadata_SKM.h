#pragma once
#include "DX12GALRIMetadata_SM.h"

struct SBASkinningJointMatrix;
class IRISkinnedMesh;



class DX12GALRIMetadata_SKM : public DX12GALRIMetadata_SM
{
public:
	DX12GALRIMetadata_SKM(GALRenderDevice* InRenderDevice, const IRISkinnedMesh* InOwnerRenderInstance);

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;

public:
	ID3D12Resource* _JointSBResource = nullptr;
	ID3D12DescriptorHeap* _JointSBDescHeap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _JointSRVDescHandle;
	SBASkinningJointMatrix* _SBAJoints = nullptr;
};
