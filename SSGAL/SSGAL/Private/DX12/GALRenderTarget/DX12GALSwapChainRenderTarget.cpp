#include "pch.h"



#include "DX12GALSwapChainRenderTarget.h"

#include "Private/DX12/Utils/SSDX12Utils.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"

DX12GALSwapChainRenderTarget::DX12GALSwapChainRenderTarget(DX12GALRenderDeviceContext* InRenderDeviceContext, HWND InhWnd, IDXGIFactory4* InFactory)
{
	_OwnerRenderDevice = InRenderDeviceContext->GetOwnerRenderDevice();
	DX12GALRenderDevice* lDX12GALRenderDevice = static_cast<DX12GALRenderDevice*>(_OwnerRenderDevice);
	ID3D12Device5* D3DDevice = lDX12GALRenderDevice->GetD3DDevice();
	ID3D12CommandQueue* D3DCommandQueue = InRenderDeviceContext->GetD3DCommandQueue();

	// Create Descriptor
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GAL_NESTED_FRAME_CNT;
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
	_ResourceSize.X = rect.right - rect.left;
	_ResourceSize.Y = rect.bottom - rect.top;
	uint32 BackBufferWidth = _ResourceSize.X;
	uint32 BackBufferHeight = _ResourceSize.Y;

	_ViewportBoxSize.WidthHeight.X = _ResourceSize.X;
	_ViewportBoxSize.WidthHeight.Y = _ResourceSize.Y;
	_ViewportBoxSize.MinDepth = 0.f;
	_ViewportBoxSize.MaxDepth = 1.f;
	_ScissorRectSize.Min = Vector2f::Zero;
	_ScissorRectSize.Max = Vector2f(_ResourceSize.X, _ResourceSize.Y);


	// Describe and create the swap chain.
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = BackBufferWidth;
		swapChainDesc.Height = BackBufferHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = GAL_NESTED_FRAME_CNT;
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

		for (uint32 i = 0; i < GAL_NESTED_FRAME_CNT; i++)
		{
			ID3D12Resource* Buffer;
			_swapChain->GetBuffer(i, IID_PPV_ARGS(&Buffer));
			Buffer->SetName(L"DX12DefaultRenderTarget::_DXRenderTargets");


			D3DDevice->CreateRenderTargetView(Buffer, nullptr, rtvHandle);
			_RTDescHandles[i] = rtvHandle;
			_DXRenderTargets[i] = Buffer;
			rtvHandle.Offset(1, _RTVDescriptorSize);
		}
	}
}

void DX12GALSwapChainRenderTarget::Release()
{
	_swapChain->Release();
	_RTVDescHeap->Release();

	for (int32 i = 0; i < GAL_NESTED_FRAME_CNT; i++)
	{
		_DXRenderTargets[i]->Release();
	}

	delete this;
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

	HRESULT hr = _swapChain->Present(uiSyncInterval, uiPresentFlags);
	_CurRenderTargetIdx = _swapChain->GetCurrentBackBufferIndex();
	return hr;
}


void DX12GALSwapChainRenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* CmdList, const Vector4f& ClearColor)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart(), _CurRenderTargetIdx, _RTVDescriptorSize);
	const float f4ClearColor[] = { ClearColor.X, ClearColor.Y, ClearColor.Z, ClearColor.W };
	CmdList->ClearRenderTargetView(rtvHandle, f4ClearColor, 0, nullptr);
}


void DX12GALSwapChainRenderTarget::ResourceBarrier(GALRenderDeviceContext* InDeviceContext, EResourceStateType From, EResourceStateType To)
{
	ID3D12GraphicsCommandList* CurCmdList = ((DX12GALRenderDeviceContext*)InDeviceContext)->GetCurrentDrawWorkerCmdList();
	ID3D12Resource* CurRenderTarget = _DXRenderTargets[_CurRenderTargetIdx];

	D3D12_RESOURCE_STATES FromD3DState = SS::DX12Util::ConvertResourceStates(From);
	D3D12_RESOURCE_STATES ToD3DState = SS::DX12Util::ConvertResourceStates(To);


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

	for (int32 i = 0; i < GAL_NESTED_FRAME_CNT; i++)
	{
		_DXRenderTargets[i]->Release();
	}


	if (FAILED(_swapChain->ResizeBuffers(GAL_NESTED_FRAME_CNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _SwapChainFlags)))
	{
		SS_INTERRUPT();
	}

	_CurRenderTargetIdx = _swapChain->GetCurrentBackBufferIndex();

	CD3DX12_CPU_DESCRIPTOR_HANDLE NewRTVHandle(_RTVDescHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < GAL_NESTED_FRAME_CNT; i++)
	{
		ID3D12Resource* Buffer = nullptr;
		_swapChain->GetBuffer(i, IID_PPV_ARGS(&Buffer));
		D3DDevice->CreateRenderTargetView(Buffer, nullptr, NewRTVHandle);
		_RTDescHandles[i] = NewRTVHandle;
		NewRTVHandle.Offset(1, _RTVDescriptorSize);
		_DXRenderTargets[i] = Buffer;
	}


	_ViewportBoxSize.WidthHeight.X = BackBufferWidth;
	_ViewportBoxSize.WidthHeight.Y = BackBufferHeight;
	_ScissorRectSize.Min.X = 0;
	_ScissorRectSize.Min.Y = 0;
	_ScissorRectSize.Max.X = BackBufferWidth;
	_ScissorRectSize.Max.Y = BackBufferHeight;
}

Vector2i32 DX12GALSwapChainRenderTarget::GetResourceSize() const
{
	return _ResourceSize;
}

int32 DX12GALSwapChainRenderTarget::GetResourceRowPitch() const
{
	SS_INTERRUPT(L"SwapChain의 Pitch를 얻어와야 할 이유가 없으며 DepthStencil과 RenderTarget중 필요로하는 Pitch가 모호합니다.");
	return 0;
}

ERenderTargetType DX12GALSwapChainRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::SwapChain;
}

ERTColorFormat DX12GALSwapChainRenderTarget::GetRTColorFormat() const
{
	return ERTColorFormat::R8G8B8A8_UNORM;
}

ID3D12Resource* DX12GALSwapChainRenderTarget::GetCurrentResource() const
{
	return _DXRenderTargets[_CurRenderTargetIdx];
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALSwapChainRenderTarget::GetCurrentRTV() const
{
	return _RTDescHandles[_CurRenderTargetIdx];
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALSwapChainRenderTarget::GetCurrentSRV() const
{
	SS_INTERRUPT("IT MUST NOT BE CALLED");
	return CD3DX12_CPU_DESCRIPTOR_HANDLE();
}

