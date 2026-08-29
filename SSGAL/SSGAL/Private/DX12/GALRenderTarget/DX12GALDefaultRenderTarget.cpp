#include "pch.h"


#include "DX12GALDefaultRenderTarget.h"

#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "Private/DX12/Utils/SSDX12Utils.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"

DX12GALDefaultRenderTarget::DX12GALDefaultRenderTarget(DX12GALRenderDevice* InRenderDevice,
                                                       const GALRenderTargetDesc& Desc, const utf16* ResourceName)
{
	HRESULT hr = S_OK;

	_InitializedDesc = Desc;

	_OwnerRenderDevice = InRenderDevice;
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();

	_ViewportBoxSize = Desc.DrawBoxSize;
	_ScissorRectSize = Desc.ScissorRectSize;
	_ResourceSize.X = Desc.ResourceWidth;
	_ResourceSize.Y = Desc.ResourceHeight;

	DXGI_FORMAT DXGIColorFormat = SS::DX12Util::ConvertColorFormat(Desc.Format);

	// Create RTV
	{
		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = DXGIColorFormat;
		ClearValue.Color[0] = 0;
		ClearValue.Color[1] = 0;
		ClearValue.Color[2] = 0;
		ClearValue.Color[3] = 0;

		D3D12_RESOURCE_DESC RTVDesc = {};
		RTVDesc.MipLevels = 1;
		RTVDesc.Format = DXGIColorFormat;
		RTVDesc.Width = _ResourceSize.X;
		RTVDesc.Height = _ResourceSize.Y;
		RTVDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		RTVDesc.DepthOrArraySize = 1;
		RTVDesc.SampleDesc.Count = 1;
		RTVDesc.SampleDesc.Quality = 0;
		RTVDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		if (Desc.bUseUAV)
		{
			RTVDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		CD3DX12_HEAP_PROPERTIES RenderTargetTypeProp = { };
		RenderTargetTypeProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		hr = D3DDevice->CreateCommittedResource(
			&RenderTargetTypeProp,
			D3D12_HEAP_FLAG_NONE,
			&RTVDesc,
			SS::DX12Util::ConvertResourceStates(Desc.InitialResourceState),
			&ClearValue,
			IID_PPV_ARGS(&_RenderTargetResource)
		);
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}

		if (ResourceName != nullptr)
		{
			_RenderTargetResource->SetName(ResourceName);
		}
		else
		{
			_RenderTargetResource->SetName(L"DX12GALDefaultRenderTarget::_RenderTargetResource");
		}


		// Create Descriptor
		{
			D3D12_DESCRIPTOR_HEAP_DESC RTHeapDesc = {};
			RTHeapDesc.NumDescriptors = 1;
			RTHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			RTHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			hr = D3DDevice->CreateDescriptorHeap(&RTHeapDesc, IID_PPV_ARGS(&_RenderTargetDescHeap)); // TODO: 
			if (FAILED(hr))
			{
				SS_INTERRUPT();
			}
			_RTVDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			_RTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart());
			D3DDevice->CreateRenderTargetView(_RenderTargetResource, nullptr, _RTVHandle);
		}

		// Alloc DescriptorHeap For Tex
		if (Desc.bUseSRV)
		{
			SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();

			_SRVDescTableChunk = DescriptorTableAllocatorForTex->AllocChunk(1, L"DX12GALDefaultRenderTarget");
			ID3D12DescriptorHeap* SRVHeap = (ID3D12DescriptorHeap*)_SRVDescTableChunk.PageContent;
			_SRVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				SRVHeap->GetCPUDescriptorHandleForHeapStart(),
				_SRVDescTableChunk.ChunkOffset,
				D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = DXGIColorFormat;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			D3DDevice->CreateShaderResourceView(_RenderTargetResource, &SRVDesc, _SRVHandle);
		}
	}

	
	// Getting Pitch
	{
		D3D12_RESOURCE_DESC Desc = _RenderTargetResource->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT descFootPrint = {};
		UINT Rows = 0;
		UINT64 RowSize = 0;
		UINT64 TotalBytes = 0;
		D3DDevice->GetCopyableFootprints(&Desc, 0, 1, 0, &descFootPrint, &Rows, &RowSize, &TotalBytes);
		_ResrouceRowPitch = descFootPrint.Footprint.RowPitch;
	}
}

void DX12GALDefaultRenderTarget::Release()
{
	if (_InitializedDesc.bUseSRV)
	{
		SSCustomMemChunkAllocator* DescriptorTableAllocatorForTex = _OwnerRenderDevice->GetDescriptorTableAllocatorForTex();
		DescriptorTableAllocatorForTex->ReleaseChunk(_SRVDescTableChunk);
	}

	_RenderTargetResource->Release();
	_RenderTargetDescHeap->Release();

	delete this;
}


Vector2i32 DX12GALDefaultRenderTarget::GetResourceSize() const
{
	return _ResourceSize;
}

int32 DX12GALDefaultRenderTarget::GetResourceRowPitch() const
{
	return _ResrouceRowPitch;
}

ERenderTargetType DX12GALDefaultRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::Default;
}

ERTColorFormat DX12GALDefaultRenderTarget::GetRTColorFormat() const
{
	return _InitializedDesc.Format;
}

ID3D12Resource* DX12GALDefaultRenderTarget::GetCurrentResource() const
{
	return _RenderTargetResource;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALDefaultRenderTarget::GetCurrentRTV() const
{
	return _RTVHandle; 
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALDefaultRenderTarget::GetCurrentSRV() const
{
	if (_InitializedDesc.bUseSRV == false)
	{
		SS_INTERRUPT();
	}

	return _SRVHandle;
}

void DX12GALDefaultRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From,
                                                 EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentDrawWorkerCmdList();
	ID3D12Resource* CurRenderTarget = _RenderTargetResource;

	D3D12_RESOURCE_STATES FromD3DState = SS::DX12Util::ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = SS::DX12Util::ConvertResourceStates(To);


	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurRenderTarget, FromD3DState, ToD3DState);
	CurCmdList->ResourceBarrier(1, &Barrier);
}

void DX12GALDefaultRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList, const Vector4f& ClearColor)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);

	const float f4ClearColor[] = { ClearColor.X, ClearColor.Y, ClearColor.Z, ClearColor.W };
	CmdList->ClearRenderTargetView(rtvHandle, f4ClearColor, 0, nullptr);
}
