#pragma once
#include "SSEngineDefault/Public/SSCommonUtil/AllocatedChunkHeader.h"

#include "Public/GALRenderAsset/GALTextureAssetWrapperBase.h"

class ITextureAssetMutable;
class DX12GALRenderDeviceContext;
class DX12GALRenderDevice;

class DX12GALTextureAssetWrapper : public GALTextureAssetWrapperBase
{
public:
	DX12GALTextureAssetWrapper(ITextureAssetMutable* OwnerAsset, DX12GALRenderDeviceContext* InExecutor);
	void Release() override;

public:
	ID3D12Resource* _TexResource = nullptr;
	AllocatedChunkHeader _DescriptorTableChunk;
	CD3DX12_CPU_DESCRIPTOR_HANDLE _SRVHandle;
};
