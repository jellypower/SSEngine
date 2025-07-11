#include "pch.h"


#include "DX12GALDefaultRenderTarget.h"

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
			hr = D3DDevice->CreateDescriptorHeap(&RTHeapDesc, IID_PPV_ARGS(&_RenderTargetDescHeap));
			if (FAILED(hr))
			{
				SS_INTERRUPT();
			}
			_RTVDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			CD3DX12_CPU_DESCRIPTOR_HANDLE RTVDescHandle(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart());
			D3DDevice->CreateRenderTargetView(_RenderTargetResource, nullptr, RTVDescHandle);
		}
	}

	// Create DSV
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

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
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&_DepthStencil));
		if(FAILED(hr))
		{
			SS_INTERRUPT();
		}
		_DepthStencil->SetName(L"DX12GALDefaultRenderTarget::_DepthStencil");

		// Create Descriptor
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

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());
		D3DDevice->CreateDepthStencilView(_DepthStencil, &depthStencilDesc, dsvHandle);
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

DX12GALDefaultRenderTarget::~DX12GALDefaultRenderTarget()
{
	_DSVHeap->Release();
	_DepthStencil->Release();

	_RenderTargetResource->Release();
	_RenderTargetDescHeap->Release();
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

void DX12GALDefaultRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From,
                                                 EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentCmdList();
	ID3D12Resource* CurRenderTarget = _RenderTargetResource;

	D3D12_RESOURCE_STATES FromD3DState = SS::DX12Util::ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = SS::DX12Util::ConvertResourceStates(To);


	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurRenderTarget, FromD3DState, ToD3DState);
	CurCmdList->ResourceBarrier(1, &Barrier);
}

void DX12GALDefaultRenderTarget::SetRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	D3D12_VIEWPORT ViewportSize;


	ViewportSize.TopLeftX = _ViewportBoxSize.LeftTop.X;
	ViewportSize.TopLeftY = _ViewportBoxSize.LeftTop.Y;
	ViewportSize.Width = _ViewportBoxSize.WidthHeight.X;
	ViewportSize.Height = _ViewportBoxSize.WidthHeight.Y;
	ViewportSize.MinDepth = _ViewportBoxSize.MinDepth;
	ViewportSize.MaxDepth = _ViewportBoxSize.MaxDepth;

	D3D12_RECT ScissorRectSize;
	ScissorRectSize.left = _ScissorRectSize.Min.X;
	ScissorRectSize.top = _ScissorRectSize.Min.Y;
	ScissorRectSize.right = _ScissorRectSize.Max.X;
	ScissorRectSize.bottom = _ScissorRectSize.Max.Y;


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	CmdList->RSSetViewports(1, &ViewportSize);
	CmdList->RSSetScissorRects(1, &ScissorRectSize);
	CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void DX12GALDefaultRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);

	constexpr FLOAT CLEAR_COLOR[] = { 0.f, 0.f, 0.f, 0.f };
	CmdList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);


	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	CmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
