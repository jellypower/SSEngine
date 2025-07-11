#include "pch.h"
#include "DX12GALTextureAssetWrapper.h"

#include <SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h>

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Public/GALRenderDevice/GALRenderDevice.h"


DX12GALTextureAssetWrapper::DX12GALTextureAssetWrapper(ITextureAsset* OwnerAsset, DX12GALRenderDevice* InOwner)
{
	_OwnerTexture = OwnerAsset;
	_OwnerRenderDevice = InOwner;
}

DX12GALTextureAssetWrapper::~DX12GALTextureAssetWrapper()
{
	_TexResource->Release();

	SSCustomMemChunkAllocator* DescriptorTableAllocator = _OwnerRenderDevice->GetDescriptorTableAllocator();
	DescriptorTableAllocator->ReleaseChunk(_DescriptorTableChunk);
}
