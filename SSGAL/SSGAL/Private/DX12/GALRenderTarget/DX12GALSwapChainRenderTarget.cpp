#include "pch.h"



#include "DX12GALSwapChainRenderTarget.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"

DX12GALSwapChainRenderTarget::DX12GALSwapChainRenderTarget(DX12GALRenderDevice* InRenderDevice, HWND InhWnd, IDXGIFactory4* InFactory)
{
	_OwnerRenderDevice = InRenderDevice;
	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();
	ID3D12CommandQueue* D3DCommandQueue = InRenderDevice->GetD3DCommandQueue();


	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = SWAP_CHAIN_FRAME_COUNT;	// SwapChain Buffer 0	| SwapChain Buffer 1
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(D3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVDescHeap))))
		{
			DEBUG_BREAK();
			return;
		}
		_RTVDescHeap->SetName(L"_DefaultViewportRenderTarget::_RTVDescHeap");
		_RTVDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	RECT rect;
	::GetClientRect(InhWnd, &rect);
	uint32 WndWidth = rect.right - rect.left;
	uint32 WndHeight = rect.bottom - rect.top;
	uint32 BackBufferWidth = WndWidth;
	uint32 BackBufferHeight = WndHeight;

	_ViewportBoxSize.WidthHeight.X = WndWidth;
	_ViewportBoxSize.WidthHeight.Y = WndHeight;
	_ViewportBoxSize.MinDepth = 0.f;
	_ViewportBoxSize.MaxDepth = 1.f;
	_ScissorRectSize.Min = Vector2f::Zero;
	_ScissorRectSize.Max = Vector2f(WndWidth, WndHeight);


	// Describe and create the swap chain.
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = BackBufferWidth;
		swapChainDesc.Height = BackBufferHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		_SwapChainFlags = swapChainDesc.Flags;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		IDXGISwapChain1* SwapChain1 = nullptr;
		if (FAILED(InFactory->CreateSwapChainForHwnd(D3DCommandQueue, InhWnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &SwapChain1)))
		{
			DEBUG_BREAK();
			return;
		}
		SwapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain));
		if (_swapChain == nullptr)
		{
			DEBUG_BREAK();
			return;
		}


		SwapChain1->Release();
		SwapChain1 = nullptr;
		_CurRenderTargetIdx = _swapChain->GetCurrentBackBufferIndex();
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart());

		for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		{
			ID3D12Resource* Buffer;
			_swapChain->GetBuffer(i, IID_PPV_ARGS(&Buffer));
			Buffer->SetName(L"DX12DefaultRenderTarget::_DXRenderTargets");


			D3DDevice->CreateRenderTargetView(Buffer, nullptr, rtvHandle);
			_DXRenderTargets.PushBack(Buffer);
			rtvHandle.Offset(1, _RTVDescriptorSize);
		}
	}

	CreateDSVDescHeap();
	RecreateDepthStencil(BackBufferWidth, BackBufferHeight);
}

DX12GALSwapChainRenderTarget::~DX12GALSwapChainRenderTarget()
{
	_DSVHeap->Release();
	_DepthStencil->Release();
	_swapChain->Release();
	_RTVDescHeap->Release();

	for (ID3D12Resource* RTItem : _DXRenderTargets)
	{
		RTItem->Release();
	}
}

HRESULT DX12GALSwapChainRenderTarget::Present()
{
	//
	// Back Buffer 화면을 Primary Buffer로 전송
	//	
	//UINT m_SyncInterval = 1;	// VSync On
	UINT m_SyncInterval = 0;	// VSync Off

	UINT uiSyncInterval = m_SyncInterval;
	UINT uiPresentFlags = 0;

	if (!uiSyncInterval)
	{
		uiPresentFlags = DXGI_PRESENT_ALLOW_TEARING;
	}

	HRESULT hr = _swapChain->Present(uiSyncInterval, uiPresentFlags); // 여기서 병목이 생긴다. 계속 쓰레드가 날아감.
	_CurRenderTargetIdx = _swapChain->GetCurrentBackBufferIndex();
	return hr;
}

void DX12GALSwapChainRenderTarget::SetRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	D3D12_VIEWPORT ViewportSize;

	
	ViewportSize.TopLeftX = _ViewportBoxSize.LeftTop.X;
	ViewportSize.TopLeftY = _ViewportBoxSize.LeftTop.Y;
	ViewportSize.Width = _ViewportBoxSize.WidthHeight.X;
	ViewportSize.Height = _ViewportBoxSize.WidthHeight.Y;
	ViewportSize.MinDepth = _ViewportBoxSize.MinDepth;
	ViewportSize.MaxDepth = _ViewportBoxSize.MaxDepth;

	D3D12_RECT ScissorRectSize;
	ScissorRectSize.left =		_ScissorRectSize.Min.X;
	ScissorRectSize.top =		_ScissorRectSize.Min.Y;
	ScissorRectSize.right =		_ScissorRectSize.Max.X;
	ScissorRectSize.bottom =	_ScissorRectSize.Max.Y;


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());


	CmdList->RSSetViewports(1, &ViewportSize);
	CmdList->RSSetScissorRects(1, &ScissorRectSize);
	CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void DX12GALSwapChainRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	constexpr float CLEAR_COLOR[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	CmdList->ClearRenderTargetView(rtvHandle, CLEAR_COLOR, 0, nullptr);
	CmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


void DX12GALSwapChainRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentCmdList();
	ID3D12Resource* CurRenderTarget = _DXRenderTargets[_CurRenderTargetIdx];

	D3D12_RESOURCE_STATES FromD3DState = ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = ConvertResourceStates(To);


	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurRenderTarget, FromD3DState, ToD3DState);
	CurCmdList->ResourceBarrier(1, &Barrier);
}

void DX12GALSwapChainRenderTarget::UpdateViewportSize(uint32 BackBufferWidth, uint32 BackBufferHeight)
{
	SS_INTERRUPT(); // TODO: Command 전부 기다리기(GALRenderDevice에 모든 명령이 끝나기를 기다리는 함수를 작성하자. )

	if (BackBufferWidth * BackBufferHeight == 0)
	{
		SS_INTERRUPT();
	}


	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();

	DXGI_SWAP_CHAIN_DESC1 desc;
	HRESULT	hr = _swapChain->GetDesc1(&desc);
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}

	for (ID3D12Resource* RenderTargetItem : _DXRenderTargets)
	{
		RenderTargetItem->Release();
	}
	_DXRenderTargets.Clear();


	if (FAILED(_swapChain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _SwapChainFlags)))
	{
		SS_INTERRUPT();
	}

	_CurRenderTargetIdx = _swapChain->GetCurrentBackBufferIndex();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		ID3D12Resource* Buffer = nullptr;
		_swapChain->GetBuffer(n, IID_PPV_ARGS(&Buffer));
		D3DDevice->CreateRenderTargetView(Buffer, nullptr, rtvHandle);
		rtvHandle.Offset(1, _RTVDescriptorSize);
		_DXRenderTargets.PushBack(Buffer);
	}

	RecreateDepthStencil(BackBufferWidth, BackBufferHeight);

	_ViewportBoxSize.WidthHeight.X = BackBufferWidth;
	_ViewportBoxSize.WidthHeight.Y = BackBufferHeight;
	_ScissorRectSize.Min.X = 0;
	_ScissorRectSize.Min.Y = 0;
	_ScissorRectSize.Max.X = BackBufferWidth;
	_ScissorRectSize.Max.Y = BackBufferHeight;
}

ERenderTargetType DX12GALSwapChainRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::SwapChain;
}

ERTColorFormat DX12GALSwapChainRenderTarget::GetRTColorFormat() const
{
	return ERTColorFormat::R8G8B8A8_UNORM;
}

void DX12GALSwapChainRenderTarget::CreateDSVDescHeap()
{
	HRESULT hr = S_OK;
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();

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

void DX12GALSwapChainRenderTarget::RecreateDepthStencil(uint32 Width, uint32 Height)
{
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();

	if (_DepthStencil != nullptr)
	{
		_DepthStencil->Release();
		_DepthStencil = nullptr;
	}


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
		Width,
		Height,
		1,
		1,
		DXGI_FORMAT_R32_TYPELESS,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	CD3DX12_HEAP_PROPERTIES depthHeapTypeProp(D3D12_HEAP_TYPE_DEFAULT);

	if (FAILED(D3DDevice->CreateCommittedResource(
		&depthHeapTypeProp,
		D3D12_HEAP_FLAG_NONE,
		&depthDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&_DepthStencil)
	)))
	{
		SS_INTERRUPT();
	}
	_DepthStencil->SetName(L"CD3D12Renderer::_DepthStencil");

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	D3DDevice->CreateDepthStencilView(_DepthStencil, &depthStencilDesc, dsvHandle);
}