#include "pch.h"
#include "DX12GALDSVRenderTarget.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "Private/DX12/Utils/SSDX12Utils.h"

DX12GALDSVRenderTarget::DX12GALDSVRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc,
                                               const utf16* ResourceName)
{
	HRESULT hr = S_OK;

	_InitializedDesc = Desc;

	_OwnerRenderDevice = InRenderDevice;
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();

	_ViewportBoxSize = Desc.DrawBoxSize;
	_ScissorRectSize = Desc.ScissorRectSize;
	_ResourceSize.X = Desc.ResourceWidth;
	_ResourceSize.Y = Desc.ResourceHeight;

	SS_ASSERT(Desc.Format == ERTColorFormat::D32_FLOAT);
	D3D12_RESOURCE_STATES D3DInitialResourceState = SS::DX12Util::ConvertResourceStates(Desc.InitialResourceState);
	

	// Create DSV
	{
		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		CD3DX12_RESOURCE_DESC depthDesc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			_ResourceSize.X,
			_ResourceSize.Y,
			1,
			1,
			DXGI_FORMAT_R32_TYPELESS,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		CD3DX12_HEAP_PROPERTIES depthHeapTypeProp(D3D12_HEAP_TYPE_DEFAULT);

		hr = D3DDevice->CreateCommittedResource(
			&depthHeapTypeProp,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3DInitialResourceState,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&_DepthStencil));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}

		if (ResourceName == nullptr)
		{
			_DepthStencil->SetName(L"DX12GALDefaultRenderTarget::_DepthStencil");
		}
		else
		{
			_DepthStencil->SetName(ResourceName);
		}
	}

	// Create Descriptor Heap for DSV
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;	// Default Depth Buffer
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(D3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_DSVHeap))))
		{
			SS_INTERRUPT();
		}

		_DSVDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	// Create DSV
	{
		_DepthStencilDescHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_DSVHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		D3DDevice->CreateDepthStencilView(_DepthStencil, &depthStencilDesc, _DepthStencilDescHandle);
	}

	// Alloc DescriptorHeap For Tex
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();

		_SRVDescTableChunk = DescriptorTableAllocatorForTex->AllocChunk(1, L"DX12GALDefaultRenderTarget::_DepthStencil");
		ID3D12DescriptorHeap* AllocatedDescHeap = (ID3D12DescriptorHeap*)_SRVDescTableChunk.PageContent;
		_SRVDescHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			AllocatedDescHeap->GetCPUDescriptorHandleForHeapStart(),
			_SRVDescTableChunk.ChunkOffset,
			D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	}

	// Create SRV
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		D3DDevice->CreateShaderResourceView(_DepthStencil, &SRVDesc, _SRVDescHandle);
	}


	// Getting Pitch
	{
		D3D12_RESOURCE_DESC Desc = _DepthStencil->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT descFootPrint = {};
		UINT Rows = 0;
		UINT64 RowSize = 0;
		UINT64 TotalBytes = 0;
		D3DDevice->GetCopyableFootprints(&Desc, 0, 1, 0, &descFootPrint, &Rows, &RowSize, &TotalBytes);
		_ResrouceRowPitch = descFootPrint.Footprint.RowPitch;
	}
}

DX12GALDSVRenderTarget::~DX12GALDSVRenderTarget()
{
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();
		DescriptorTableAllocatorForTex->ReleaseChunk(_SRVDescTableChunk);
	}

	_DSVHeap->Release();
	_DepthStencil->Release();
}

Vector2i32 DX12GALDSVRenderTarget::GetResourceSize() const
{
	return _ResourceSize;
}

int32 DX12GALDSVRenderTarget::GetResourceRowPitch() const
{
	return _ResrouceRowPitch;
}

ERenderTargetType DX12GALDSVRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::DepthStencil;
}

ERTColorFormat DX12GALDSVRenderTarget::GetRTColorFormat() const
{
	return ERTColorFormat::D32_FLOAT;
}

ID3D12Resource* DX12GALDSVRenderTarget::GetCurrentResource() const
{
	return _DepthStencil;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALDSVRenderTarget::GetCurrentRTV() const
{
	// DSVRenderTarget객체는 DepthStencilView를 리턴해줌
	return _DepthStencilDescHandle;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALDSVRenderTarget::GetCurrentSRV() const
{
	return _SRVDescHandle;
}

void DX12GALDSVRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From,
                                             EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentDrawWorkerCmdList();
	ID3D12Resource* CurRenderTarget = _DepthStencil;

	D3D12_RESOURCE_STATES FromD3DState = SS::DX12Util::ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = SS::DX12Util::ConvertResourceStates(To);

	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurRenderTarget, FromD3DState, ToD3DState);
	CurCmdList->ResourceBarrier(1, &Barrier);
}


void DX12GALDSVRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	CmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
