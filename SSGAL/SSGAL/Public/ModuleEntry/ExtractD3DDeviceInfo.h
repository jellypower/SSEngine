#pragma once
#ifdef _WINDOWS
#include "SSGAL/ModuleExportKeyword.h"


class GALRenderTarget;
class GALRenderDeviceContext;
class GALRenderDevice;

SSGAL_MODULE_NATIVE void QueryDX12GALDeviceContextInfo(
	GALRenderDeviceContext* DeviceToQuery,
	ID3D12Device** OutDevice,
	ID3D12CommandQueue** OutCommandQueue);
typedef decltype(&QueryDX12GALDeviceContextInfo) FuncPtr_QueryDX12GALDeviceContextInfo;


SSGAL_MODULE_NATIVE void QueryD3DSwapChainInfo(
	GALRenderTarget* SwapChainToQuery,
	ID3D12Resource** OutBackBuffer,
	D3D12_CPU_DESCRIPTOR_HANDLE* OutBackBufferDescHeap);
typedef decltype(&QueryD3DSwapChainInfo) FuncPtr_QueryD3DSwapChainInfo;

#endif