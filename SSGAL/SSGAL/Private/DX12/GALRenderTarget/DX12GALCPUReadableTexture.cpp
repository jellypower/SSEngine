#include "pch.h"
#include "DX12GALCPUReadableTexture.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/Utils/SSDX12Utils.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSGAL/Public/GALRenderTarget/GALRTCommonEnums.h"

DX12GALCPUReadableTexture::DX12GALCPUReadableTexture(DX12GALRenderDevice* InOwnerDevice, ERTColorFormat InColorFormat, Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName)
{
	HRESULT hr = S_OK;

	_OwnerRenderDevice = InOwnerDevice;
	_ResourceWidthHeight = InWidthHeight;
	_ColorFormat = InColorFormat;
	_Pitch = Pitch;

	ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();


	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_UNKNOWN; // CPU에서 읽으려면 타입이 UNKNOWN이어야 한다. ColorFormat을 파라미터로 받는건 리소스 사이즈 계산을 위해서이다.
	ResourceDesc.Width = _Pitch * _ResourceWidthHeight.Y * SS::DX12Util::GetColorFormatSizeInByte(InColorFormat);
	ResourceDesc.Height = 1;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.SampleDesc.Count = 1;
	ResourceDesc.SampleDesc.Quality = 0;
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	CD3DX12_HEAP_PROPERTIES RenderTargetTypeProp = { };
	RenderTargetTypeProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

	hr = D3DDevice->CreateCommittedResource(
		&RenderTargetTypeProp,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		SS::DX12Util::ConvertResourceStates(EResourceStateType::CopyDest),
		nullptr,
		IID_PPV_ARGS(&_TexResource)
	);
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}

	if (ResourceName != nullptr)
	{
		_TexResource->SetName(ResourceName);
	}
	else
	{
		_TexResource->SetName(L"GALCPUReadableTexture");
	}
}

void DX12GALCPUReadableTexture::Release()
{
	_TexResource->Release();

	delete this;
}

const Vector2i32& DX12GALCPUReadableTexture::GetResourceWidthHeight() const
{
	return _ResourceWidthHeight;
}

const ERTColorFormat DX12GALCPUReadableTexture::GetColorFormat() const
{
	return _ColorFormat;
}

void* DX12GALCPUReadableTexture::GetDataAt(int32 X, int32 Y) const
{
	if (X < 0 || X >= _ResourceWidthHeight.X || Y < 0 || Y >= _ResourceWidthHeight.Y)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	byte* DataToRead = _MappedData;

	DataToRead = DataToRead + (_Pitch * Y);
	DataToRead = DataToRead + (X * SS::DX12Util::GetColorFormatSizeInByte(_ColorFormat));
	return DataToRead;
}

void* DX12GALCPUReadableTexture::GetDataAtRatio(float RatioX, float RatioY) const
{
	if (RatioX < 0 || RatioX > 1.f || RatioY < 0 || RatioY > 1.f)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	int32 X = (float)_ResourceWidthHeight.X * RatioX;
	int32 Y = (float)_ResourceWidthHeight.Y * RatioY;

	return GetDataAt(X, Y);
}

void DX12GALCPUReadableTexture::BeginRead()
{
	HRESULT hr = _TexResource->Map(0, nullptr, reinterpret_cast<void**>(&_MappedData));
	if (FAILED(hr))
	{
		SS_INTERRUPT();
	}
}

void DX12GALCPUReadableTexture::EndRead()
{
	_TexResource->Unmap(0, nullptr);
	_MappedData = nullptr;
}
