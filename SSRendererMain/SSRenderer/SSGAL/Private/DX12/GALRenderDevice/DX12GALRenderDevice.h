#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>

#include "SSGAL/Private/PCommon/GALRenderDevice/PCommonGALRenderDevice.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"


struct GALRenderTargetDesc;
class IRenderInstance;

class DX12GALRenderDevice : public PCommonGALRenderDevice
{
public:
	DX12GALRenderDevice(HINSTANCE InhInst, HWND InhWnd, bool EnableDebugLayer, bool EnableGPUBaseValidataion);
	virtual ~DX12GALRenderDevice();

public:
	void BeginRender() override;
	void EndRender() override;


	virtual ERenderDevicePlatnform GetRenderDevicePlatform() const override;


public:

	virtual GALRenderDeviceContext* CreateRenderDeviceContext() override;
	virtual GALRenderTarget* CreateRenderTarget(const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr) override;

	virtual void ExecuteRenderContext(GALRenderDeviceContext* DeviceContext) override;



	ID3D12Device5* GetD3DDevice() const { return _D3DDevice; }
	ID3D12CommandQueue* GetD3DCommandQueue() const { return _D3DCommandQueue; }

protected:
	virtual void WaitForFence() override;
	virtual void FenceFrame() override;

private:
	void Present();


private:
	HINSTANCE _hInst = NULL;
	HWND _hWnd = NULL;

	ID3D12Device5* _D3DDevice = nullptr;
	ID3D12CommandQueue* _D3DCommandQueue = nullptr;


	ID3D12Fence* _Fence = nullptr;
	HANDLE _FenceEvent = nullptr;
	uint32 _CurRenderTargetIdx = 0;
};
