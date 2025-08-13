#include "pch.h"
#include "DX12GALUAVRenderTarget.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/DX12/Utils/SSDX12Utils.h"

DX12GALUAVRenderTarget::DX12GALUAVRenderTarget(DX12GALRenderDevice* InRenderDevice, const GALRenderTargetDesc& Desc, const utf16* ResourceName):
	DX12GALDefaultRenderTarget(InRenderDevice, Desc, ResourceName)
{
	HRESULT hr;

	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_OwnerRenderDevice)->GetD3DDevice();
	{
		D3D12_DESCRIPTOR_HEAP_DESC RTHeapDesc = {};
		RTHeapDesc.NumDescriptors = 1;
		RTHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		RTHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = D3DDevice->CreateDescriptorHeap(&RTHeapDesc, IID_PPV_ARGS(&_UAVDescHeap));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}
		_UAVDescHeap->SetName(ResourceName);

		_UAVDescSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		const D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
			.Format = SS::DX12Util::ConvertColorFormat(Desc.Format),
			.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
			.Texture2D = {
				.MipSlice = 0,
				.PlaneSlice = 0
			}
		};
		_UAVDescHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_UAVDescHeap->GetCPUDescriptorHandleForHeapStart());
		D3DDevice->CreateUnorderedAccessView(
			_RenderTargetResource, 
			nullptr,
			&uavDesc,
			_UAVDescHandle);
	}
}

DX12GALUAVRenderTarget::~DX12GALUAVRenderTarget()
{
	_UAVDescHeap->Release();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GALUAVRenderTarget::GetCurrentUAV() const
{
	return _UAVDescHandle;
}

ERenderTargetType DX12GALUAVRenderTarget::GetRenderTargetType() const
{
	return ERenderTargetType::Default_UAV;
}
