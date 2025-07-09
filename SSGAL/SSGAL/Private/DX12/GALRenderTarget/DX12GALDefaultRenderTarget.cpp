#include "pch.h"


#include "DX12GALDefaultRenderTarget.h"

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
	_ResourceWidth = Desc.ResourceWidth;
	_ResourceHeight = Desc.ResourceHeight;

	DXGI_FORMAT DXGIColorFormat = ConvertColorFormat(Desc.Format);


	D3D12_CLEAR_VALUE ClearValue = {};
	ClearValue.Format = DXGIColorFormat;
	ClearValue.Color[0] = 0;
	ClearValue.Color[1] = 0;
	ClearValue.Color[2] = 0;
	ClearValue.Color[3] = 0;

	D3D12_RESOURCE_DESC RTVDesc = {};
	RTVDesc.MipLevels = 1;
	RTVDesc.Format = DXGIColorFormat;
	RTVDesc.Width = _ResourceWidth;
	RTVDesc.Height = _ResourceHeight;
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
		ConvertResourceStates(Desc.InitialResourceState),
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

DX12GALDefaultRenderTarget::~DX12GALDefaultRenderTarget()
{
	_RenderTargetResource->Release();
	_RenderTargetDescHeap->Release();
}

ERenderTargetType DX12GALDefaultRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::Default;
}

ERTColorFormat DX12GALDefaultRenderTarget::GetRTColorFormat() const
{
	return _InitializedDesc.Format;
}

void DX12GALDefaultRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From,
                                                 EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentCmdList();
	ID3D12Resource* CurRenderTarget = _RenderTargetResource;

	D3D12_RESOURCE_STATES FromD3DState = ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = ConvertResourceStates(To);


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


	CmdList->RSSetViewports(1, &ViewportSize);
	CmdList->RSSetScissorRects(1, &ScissorRectSize);
	CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void DX12GALDefaultRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RenderTargetDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);

	constexpr float CLEAR_COLOR[] = { 0.f, 0.f};
	CmdList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
}
