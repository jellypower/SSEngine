#pragma once
#include <d3d12.h>

#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"
#include "SSGAL/Public/GPURenderInstance/GALRIMetadata.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

struct CBARenderEnvParam;
struct CBAModelBuffer;
class DX12GALRenderDevice;
class GALRenderDevice;

class DX12GALRIMetadata_SM : public GALRIMetadata
{
public:
	AllocatedChunkHeader _DescriptorTableChunk;

	AllocatedChunkHeader _ModelCBChunk;
	CBAModelBuffer* _ModelCBSysMemAddr;
	D3D12_GPU_VIRTUAL_ADDRESS _ModelCBGPUMemAddr;

	AllocatedChunkHeader _RenderEnvCBChunk;
	CBARenderEnvParam* _RenderEnvCBSysMemAddr;
	D3D12_GPU_VIRTUAL_ADDRESS _RenderEnvCBGPUMemAddr;

	int32 _SubMeshCntCache;
	D3D12_CPU_DESCRIPTOR_HANDLE	_CPUDescriptorHandle[SUBMESH_COUNT_MAX];
	D3D12_GPU_DESCRIPTOR_HANDLE	_GPUDescriptorHandle[SUBMESH_COUNT_MAX];

private:
	DX12GALRenderDevice* _OwnerRenderDevice;

public:
	DX12GALRIMetadata_SM(GALRenderDevice* InRenderDevice, const BasicRenderInstance* InOwnerRenderInstance);
	virtual ~DX12GALRIMetadata_SM();

	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;
};
