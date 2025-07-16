#pragma once
// #include <d3d12.h>
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"
#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

class IRIMesh;
struct CBARenderEnvParam;
struct CBAModelBuffer;
class DX12GALRenderDevice;
class GALRenderDevice;

// GAL RenderInstance Metadata Static Mesh
class DX12GALRIMetadata_SM : public GALRIMetadata
{
public:
	AllocatedChunkHeader _ModelCBChunk;
	CBAModelBuffer* _ModelCBSysMemAddr;
	D3D12_GPU_VIRTUAL_ADDRESS _ModelCBGPUMemAddr;

	int32 _SubMeshCntCache;

private:
	DX12GALRenderDevice* _OwnerRenderDevice;

public:
	DX12GALRIMetadata_SM(GALRenderDevice* InRenderDevice, const IRIMesh* InOwnerRenderInstance);
	virtual ~DX12GALRIMetadata_SM();

	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;
};
