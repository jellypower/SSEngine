#pragma once
// #include <d3d12.h>
// #include <dxgi1_4.h>
// #include <dxgidebug.h>

#include "SSGAL/Private/PCommon/GALRenderDevice/PCommonGALRenderDevice.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


struct GALRenderTargetDesc;
class IRenderInstance;

class DX12GALRenderDevice : public PCommonGALRenderDevice
{
public:
	DX12GALRenderDevice(bool EnableDebugLayer, bool EnableGPUBaseValidataion);
	virtual ~DX12GALRenderDevice();

public:

	virtual ERenderDevicePlatnform GetRenderDevicePlatform() const override;


public:
	virtual GALRenderDeviceContext* CreateRenderDeviceContext() override;
	virtual GALRenderTarget* CreateRenderTarget(const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr) override;
	virtual GALRenderTarget* CreateDepthStencilView(const GALRenderTargetDesc& Desc, const utf16* ResourceName) override;
	virtual GALCPUReadableTexture* CreateCPUReadableTexture(ERTColorFormat InColorFormat, Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName = nullptr) override;
	virtual GALPPCDeferredShading* CreateDeferredShadingPostProcessContext() override;


	ID3D12Device5* GetD3DDevice() const { return _D3DDevice; }
	IDXGIFactory4* GetDXGIFactory() const { return _DXGIFactory; }


private:
	ID3D12Device5* _D3DDevice = nullptr;
	IDXGIFactory4* _DXGIFactory = nullptr;
};
