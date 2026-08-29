#pragma once
#include "Public/GALRenderInstance/Light/GALRIShadowMapMetadata.h"

#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

struct CBARenderEnvParam;
class IRenderLightDirectional;
class DX12GALRenderDevice;
class GALRenderTarget;


class DX12GALRIDirectionalLightShadowMapMetadata : public GALRIShadowMapMetadata
{
public:
	AllocatedChunkHeader _ShadowMapCBChunk;
	CBARenderEnvParam* _ShadowMapCBSysMemAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS _ShadowMapCBGPUMemAddr;

private:
	GALRenderTarget* _ShadowMap = nullptr; // 이 놈은 2개 필요 없음 -> DX12GALRIDirectionalLightShadowMapMetadata 자체를 2개 만들면 됨
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;


public:
	DX12GALRIDirectionalLightShadowMapMetadata(DX12GALRenderDevice* InRenderDevice, IRenderLightDirectional* InOwnerRenderLight);
	void Release() override;

public:
	virtual ERenderInstanceType GetMetadataRenderInstanceType() override;
	virtual ELightType GetLightType() const override;
	virtual GALRenderTarget* GetShadowMap() const override;
};
