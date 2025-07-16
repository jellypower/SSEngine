#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "Public/GALRenderInstance/GALRWMetaData.h"

class DX12GALRenderDevice;
struct CBARenderEnvParam;

class DX12GALRWMetaData : public GALRWMetaData
{
public:
	CBARenderEnvParam* _RenderEnvCBSysMemAddr;
	D3D12_GPU_VIRTUAL_ADDRESS _RenderEnvCBGPUMemAddr;

private:
	AllocatedChunkHeader _RenderEnvCBChunk;
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;

public:
	DX12GALRWMetaData(DX12GALRenderDevice* InRenderDevice, IRenderWorld* InOwnerRenderWorld);
	virtual ~DX12GALRWMetaData();
};
