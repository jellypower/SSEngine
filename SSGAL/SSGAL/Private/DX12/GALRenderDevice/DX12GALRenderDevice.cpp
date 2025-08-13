#include "pch.h"

#include "DX12GALRenderDevice.h"

#include "DX12GALRenderDeviceContext.h"
#include "Private/DX12/GALRenderTarget/DX12GALCPUReadableTexture.h"
#include "Private/DX12/GALRenderTarget/DX12GALDSVRenderTarget.h"
#include "Private/DX12/GALRenderTarget/DX12GALUAVRenderTarget.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourceAllocator.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12DescriptorHeapCustomAllocator.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDefaultRenderTarget.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12GALShaderPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignaturePool.h"
#include "SSGAL/Public/SSGALInlineSettings.h"



DX12GALRenderDevice::DX12GALRenderDevice(HINSTANCE InhInst, HWND InhWnd, bool EnableDebugLayer, bool EnableGPUBaseValidataion)
{
	_hInst = InhInst;
	_hWnd = InhWnd;

	BOOL Result = FALSE;

	HRESULT hr = S_OK;
	ID3D12Debug* DebugController = nullptr;
	IDXGIFactory4* Factory = nullptr;
	IDXGIAdapter1* Adapter = nullptr;
	DXGI_ADAPTER_DESC1 AdapterDesc = {};

	DWORD CreateFlags = 0;
	DWORD CreateFactoryFlags = 0;


	if (EnableDebugLayer)
	{
		// Enable the D3D12 debug layer.
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
		}
		CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
		if (EnableGPUBaseValidataion)
		{
			ID3D12Debug5* DebugController5 = nullptr;
			if (S_OK == DebugController->QueryInterface(IID_PPV_ARGS(&DebugController5)))
			{
				DebugController5->SetEnableGPUBasedValidation(TRUE);
				DebugController5->SetEnableAutoName(TRUE);
				DebugController5->Release();
			}
		}
	}

	CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&Factory));

	D3D_FEATURE_LEVEL	featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DWORD	FeatureLevelNum = _countof(featureLevels);

	for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
	{
		UINT adapterIndex = 0;
		while (DXGI_ERROR_NOT_FOUND != Factory->EnumAdapters1(adapterIndex, &Adapter))
		{
			Adapter->GetDesc1(&AdapterDesc);

			if (SUCCEEDED(D3D12CreateDevice(Adapter, featureLevels[featerLevelIndex], IID_PPV_ARGS(&_D3DDevice))))
			{
				_D3DDevice->SetName(L"D3DDevice");
				goto lb_loop;
			}
			Adapter->Release();
			Adapter = nullptr;
			adapterIndex++;
		}
	}
lb_loop:


	hr = _D3DDevice->CreateFence(_CurFrameCnt, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_Fence));
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}
	_Fence->SetName(L"RenderDeviceFence");

	_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (FAILED(_FenceEvent))
	{
		SS_INTERRUPT();
	}

	if (DebugController != nullptr)
	{
		ID3D12InfoQueue* InfoQueue = nullptr;
		_D3DDevice->QueryInterface(IID_PPV_ARGS(&InfoQueue));
		if (InfoQueue != nullptr)
		{
			InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

			D3D12_MESSAGE_ID hide[] =
			{
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				// Workarounds for debug layer issues on hybrid-graphics systems
				D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = (UINT)_countof(hide);
			filter.DenyList.pIDList = hide;
			InfoQueue->AddStorageFilterEntries(&filter);

			InfoQueue->Release();
			InfoQueue = nullptr;
		}
		else
		{
			DEBUG_BREAK();
		}
	}


	// Create Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hr = _D3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_D3DCommandQueue));
		if (FAILED(hr))
		{
			DEBUG_BREAK();
			return;
		}
		_D3DCommandQueue->SetName(L"D3DCommandQueue");
	}

	_DefaultViewportRenderTarget = DBG_NEW DX12GALSwapChainRenderTarget(this, InhWnd, Factory);

	if (DebugController)
	{
		DebugController->Release();
		DebugController = nullptr;
	}
	if (Adapter)
	{
		Adapter->Release();
		Adapter = nullptr;
	}
	if (Factory)
	{
		Factory->Release();
		Factory = nullptr;
	}


	constexpr int32 DESCRIPTOR_HEAP_PAGE_SIZE = 1024 * 10; // 10 KB
	_DescriptorTableAllocator = DBG_NEW DX12DescriptorHeapCustomAllocator(
		this,
		DESCRIPTOR_HEAP_PAGE_SIZE,
		1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"DX12GALRenderDevice::_DescriptorTableAllocator");
	_DescriptorTableAllocator->ReserveDefaultPage(2);


	_DescriptorTableAllocatorForTex = DBG_NEW DX12DescriptorHeapCustomAllocator(
		this,
		DESCRIPTOR_HEAP_PAGE_SIZE,
		1,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"DX12GALRenderDevice::_DescriptorTableAllocatorForTex");
	_DescriptorTableAllocatorForTex->ReserveDefaultPage(2);



	constexpr int32 CONSTANTBUFFER_RESOURCE_PAGE_SIZE = GAL_RESOURCE_DEFAULT_ALIGNMENT_SIZE * 16; // 1MB
	_ConstantBufferResourceAllocator = DBG_NEW DX12ConstantBufferResourceCustomAllocator(
		this,
		CONSTANTBUFFER_RESOURCE_PAGE_SIZE,
		GAL_CONSTANTBUFFER_MIN_SIZE,
		L"DX12GALRenderDevice::_ConstantBufferResourceAllocator");
	_ConstantBufferResourceAllocator->ReserveDefaultPage(2);

	_rootSignaturePool = DBG_NEW DX12RootSignaturePool(this);
	_rootSignaturePool->InstantiateAllRootSignatures();

	_ShaderPool = DBG_NEW DX12GALShaderPool();
	_ShaderPool->Initialize();

	_PSOPool = DBG_NEW DX12PSOPool(this);
}

DX12GALRenderDevice::~DX12GALRenderDevice()
{
	// DX12GALRenderDevice를 제거하기 전에는 win32 API에서 WM_QUIT신호가 오기 때문에 PerFrame에서 BeginRender, EndRender를 부르지 않는다.
	// 그래서 직접 Fence를 박아서 작업이 완료되기를 기다려야 한다.
	DX12GALRenderDevice::FenceFrame();  
	DX12GALRenderDevice::WaitForFence();

	_PSOPool->ReleaseAllPSO();
	delete _PSOPool;
	_PSOPool = nullptr;

	delete _ShaderPool;

	_rootSignaturePool->ReleaseAllRoogSignatures();
	delete _rootSignaturePool;
	_rootSignaturePool = nullptr;

	_ConstantBufferResourceAllocator->ReleaseDefaultPages();
	delete _ConstantBufferResourceAllocator;

	_DescriptorTableAllocatorForTex->ReleaseDefaultPages();
	delete _DescriptorTableAllocatorForTex;

	_DescriptorTableAllocator->ReleaseDefaultPages();
	delete _DescriptorTableAllocator;

	delete _DefaultViewportRenderTarget;

	_D3DCommandQueue->Release();

	_Fence->Release();
	CloseHandle(_FenceEvent);

	int32 RefCnt = _D3DDevice->Release();
	if (RefCnt > 0)
	{
		IDXGIDebug1* pDebug = nullptr;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
		{
			pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
			pDebug->Release();
		}
		DEBUG_BREAK();
	}
}

void DX12GALRenderDevice::BeginRender()
{
	WaitForFence();
}

void DX12GALRenderDevice::EndRender()
{
	FenceFrame();
	Present();
}

ERenderDevicePlatnform DX12GALRenderDevice::GetRenderDevicePlatform() const
{
	return ERenderDevicePlatnform::DX12Raster;
}

void DX12GALRenderDevice::ExecuteRenderContext(GALRenderDeviceContext* DeviceContext)
{
	// Valid 체크
	if (this != DeviceContext->GetOwnerRenderDevice())
	{
		SS_INTERRUPT();
	}

	// 필요한 데이터 뽑아오기
	DX12GALRenderDeviceContext* DX12DeviceContext = (DX12GALRenderDeviceContext*)DeviceContext;
	const SS::PooledList<ID3D12GraphicsCommandList*>& CommandLists = DX12DeviceContext->GetDrawWorkerCommandLists();
	int32 Size = CommandLists.GetSize();
	ID3D12CommandList* const* Lists = (ID3D12CommandList* const*)CommandLists.GetData();

	// 실행
	_D3DCommandQueue->ExecuteCommandLists(Size, Lists);
	_ExecutedDeviceContext.PushBack(DX12DeviceContext);
}

void DX12GALRenderDevice::Present()
{
	DX12GALSwapChainRenderTarget* DX12SwapChain = (DX12GALSwapChainRenderTarget*)_DefaultViewportRenderTarget;
	HRESULT hr = DX12SwapChain->Present();
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}
}

GALRenderDeviceContext* DX12GALRenderDevice::CreateRenderDeviceContext()
{
	ID3D12CommandAllocator* NewCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* NewCommandList = nullptr;

	DX12GALRenderDeviceContext* NewDeviceContext = DBG_NEW DX12GALRenderDeviceContext(this, SWAP_CHAIN_FRAME_COUNT);

	if (NewDeviceContext->IsValid())
	{
		return NewDeviceContext;
	}

	delete NewDeviceContext;
	return nullptr;
}

GALRenderTarget* DX12GALRenderDevice::CreateRenderTarget(const GALRenderTargetDesc& Desc, const utf16* ResourceName)
{
	DX12GALDefaultRenderTarget* NewRenderTarget = nullptr;
	if (Desc.bUseUAV)
	{
		NewRenderTarget = DBG_NEW DX12GALUAVRenderTarget(this, Desc, ResourceName);
	}
	else
	{
		NewRenderTarget = DBG_NEW DX12GALDefaultRenderTarget(this, Desc, ResourceName);
	}

	return NewRenderTarget;
}

GALRenderTarget* DX12GALRenderDevice::CreateDepthStencilView(const GALRenderTargetDesc& Desc, const utf16* ResourceName)
{
	DX12GALDSVRenderTarget* NewDSV = DBG_NEW DX12GALDSVRenderTarget(this, Desc, ResourceName);
	return NewDSV;
}

GALCPUReadableTexture* DX12GALRenderDevice::CreateCPUReadableTexture(ERTColorFormat InColorFormat, Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName)
{
	DX12GALCPUReadableTexture* NewReadableTex = DBG_NEW DX12GALCPUReadableTexture(this, InColorFormat, InWidthHeight, Pitch, ResourceName);
	return NewReadableTex;
}


void DX12GALRenderDevice::WaitForFence()
{
	uint64 CompletedValue = _Fence->GetCompletedValue();
	if (CompletedValue < _CurFrameCnt)
	{
		_Fence->SetEventOnCompletion(_CurFrameCnt, _FenceEvent);
		WaitForSingleObject(_FenceEvent, INFINITE);
	}

	_ExecutedDeviceContext.Clear();
}

void DX12GALRenderDevice::FenceFrame()
{
	_CurFrameCnt++;
	_D3DCommandQueue->Signal(_Fence, _CurFrameCnt);
}
