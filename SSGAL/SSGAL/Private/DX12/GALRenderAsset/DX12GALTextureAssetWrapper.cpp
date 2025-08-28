#include "pch.h"
#include "DX12GALTextureAssetWrapper.h"

#include <SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h>
#include <SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/ITextureAssetMutable.h>
#include <SSRenderer/Public/RenderAsset/RenderAssetType/ITextureAsset.h>

#include "Private/DX12/DX12CommonUtils/DDSTextureLoader12/DDSTextureLoader12.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALResourceUpdater/DX12GALResourceUpdater.h"
#include "Public/GALRenderDevice/GALRenderDevice.h"


DX12GALTextureAssetWrapper::DX12GALTextureAssetWrapper(ITextureAssetMutable* OwnerAsset, DX12GALRenderDeviceContext* InExecutor)
{
	_OwnerTexture = OwnerAsset;
	_OwnerRenderDevice = static_cast<DX12GALRenderDevice*>(InExecutor->GetOwnerRenderDevice());

	HRESULT hr = S_OK;

	DX12GALResourceUpdater* DX12ResourceUpdater = static_cast<DX12GALResourceUpdater*>(InExecutor->GetResourceUpdater());
	DX12GALRenderDevice* OwnerDX12RenderDevice = static_cast<DX12GALRenderDevice*>(_OwnerRenderDevice);
	SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = OwnerDX12RenderDevice->GetDescriptorTableAllocatorForTex();
	ID3D12Device5* D3DDevice = OwnerDX12RenderDevice->GetD3DDevice();
	ID3D12GraphicsCommandList* CurCommandList = InExecutor->GetCurrentDrawWorkerCmdList();

	const utf16* TexturePath = OwnerAsset->GetAssetPath().C_Str();
	const utf16* TextureName = OwnerAsset->GetAssetName().C_Str();


	D3D12_RESOURCE_DESC textureDesc = {};
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
	bool bIsCubeMap = false;
	if (FAILED(LoadDDSTextureFromFile(D3DDevice, TexturePath, &_TexResource, ddsData, subresouceData, 0, nullptr, &bIsCubeMap)))
	{
		DEBUG_BREAK();
		if (_TexResource != nullptr)
		{
			_TexResource->Release();
			_TexResource = nullptr;
		}
	}
	textureDesc = _TexResource->GetDesc();

	const D3D12_SUBRESOURCE_DATA* SrcData = subresouceData.data();
	UINT NumSubResources = (UINT)subresouceData.size();
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(_TexResource, 0, NumSubResources);

	if (bIsCubeMap)
	{
		SS_ASSERT(_OwnerTexture->GetTextureType() == ETextureType::CubeMap);
	}


	hr = DX12ResourceUpdater->UpdateTexture(
		CurCommandList,
		_TexResource,
		SrcData,
		NumSubResources,
		uploadBufferSize,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		if (_TexResource != nullptr)
		{
			_TexResource->Release();
			_TexResource = nullptr;
		}
	}
	_TexResource->SetName(TextureName);

	_DescriptorTableChunk = DescriptorTableAllocatorForTex->AllocChunk(1, _OwnerTexture->GetAssetName());
	ID3D12DescriptorHeap* AllocatedDescHeap = (ID3D12DescriptorHeap*)_DescriptorTableChunk.PageContent;
	_SRVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		AllocatedDescHeap->GetCPUDescriptorHandleForHeapStart(),
		_DescriptorTableChunk.ChunkOffset,
		D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));


	if (bIsCubeMap == false)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = textureDesc.Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		D3DDevice->CreateShaderResourceView(_TexResource, &SRVDesc, _SRVHandle);
	}
	else
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = textureDesc.Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		D3DDevice->CreateShaderResourceView(_TexResource, &SRVDesc, _SRVHandle);
	}

}

DX12GALTextureAssetWrapper::~DX12GALTextureAssetWrapper()
{
	_TexResource->Release();

	SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();
	DescriptorTableAllocatorForTex->ReleaseChunk(_DescriptorTableChunk);
}
