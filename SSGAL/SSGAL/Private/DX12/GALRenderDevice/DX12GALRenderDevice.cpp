#include "pch.h"

#include "DX12GALRenderDevice.h"

#include <SSRenderer/Public/RenderInstance/IRenderInstance.h>

#include "DX12GALRenderDeviceContext.h"
#include "Private/DX12/DX12CommonUtils/DX12TransientConstantBufferAllocator.h"
#include "Private/DX12/GALPostProcessContext/DX12GALPPCDeferredShading.h"
#include "Private/DX12/GALRenderInstance/DX12GALRIMetadata_SKM.h"
#include "Private/DX12/GALRenderTarget/DX12GALCPUReadableTexture.h"
#include "Private/DX12/GALRenderTarget/DX12GALDSVRenderTarget.h"
#include "Private/DX12/GALRenderTarget/DX12GALUAVRenderTarget.h"
#include "Private/PCommon/TestCodes/GALTestCodes.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12ConstantBufferResourceAllocator.h"
#include "SSGAL/Private/DX12/DX12CommonUtils/DX12DescriptorHeapCustomAllocator.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALDefaultRenderTarget.h"
#include "SSGAL/Private/DX12/GALRenderTarget/DX12GALSwapChainRenderTarget.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12GALShaderPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12PSOPool.h"
#include "SSGAL/Private/DX12/GALWrapper/DX12RootSignaturePool.h"
#include "SSGAL/Public/SSGALInlineSettings.h"



DX12GALRenderDevice::DX12GALRenderDevice(bool EnableDebugLayer, bool EnableGPUBaseValidataion):
	PCommonGALRenderDevice(EnableDebugLayer)
{

	BOOL Result = FALSE;

	HRESULT hr = S_OK;
	ID3D12Debug* DebugController = nullptr;
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

	CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&_DXGIFactory));

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
		while (DXGI_ERROR_NOT_FOUND != _DXGIFactory->EnumAdapters1(adapterIndex, &Adapter))
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

	if (_EnableDebug)
	{
		TestCustomChunkAllocator(this);
	}
}

DX12GALRenderDevice::~DX12GALRenderDevice()
{
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

	_DXGIFactory->Release();

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
}

void DX12GALRenderDevice::EndRender()
{
}

ERenderDevicePlatnform DX12GALRenderDevice::GetRenderDevicePlatform() const
{
	return ERenderDevicePlatnform::DX12Raster;
}

GALRenderDeviceContext* DX12GALRenderDevice::CreateRenderDeviceContext()
{
	DX12GALRenderDeviceContext* NewDeviceContext = DBG_NEW DX12GALRenderDeviceContext(this);

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

GALPPCDeferredShading* DX12GALRenderDevice::CreateDeferredShadingPostProcessContext()
{
	DX12GALPPCDeferredShading* NewPostProcessContext = DBG_NEW DX12GALPPCDeferredShading(this);
	return NewPostProcessContext;
}

void DX12GALRenderDevice::SyncGALRIMetadataWithRI(IRenderInstance* RIToSync)
{
	if (RIToSync->GetGALMetadata() == nullptr)
	{
		return;
	}

	ERenderInstanceType RIType = RIToSync->GetRIType();

	if (RIType == ERenderInstanceType::SkinnedMesh)
	{
		DX12GALRIMetadata_SKM* GALRISkinned = static_cast<DX12GALRIMetadata_SKM*>(RIToSync->GetGALMetadata());
		SS_ASSERT(GALRISkinned->GetMetadataRenderInstanceType() == ERenderInstanceType::SkinnedMesh);
		GALRISkinned->SyncBonePose();
	}
}