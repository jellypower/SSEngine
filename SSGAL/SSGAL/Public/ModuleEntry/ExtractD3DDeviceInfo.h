#pragma once
#ifdef _WINDOWS
#include "SSGAL/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"


class GALRenderDevice;

SSGAL_MODULE_NATIVE void ExtractD3DDeviceInfo(
	GALRenderDevice* DeviceToExtract,
	ID3D12Device** OutDevice,
	ID3D12CommandQueue** OutCommandQueue,
	int32* OutNestedFrameCnt);
typedef decltype(&ExtractD3DDeviceInfo) FuncPtr_ExtractD3DDeviceInfo;


SSGAL_MODULE_NATIVE void QueryCurrentD3DGALDeviceContext(
	GALRenderDevice* DeviceToQuery,
	ID3D12Resource** OutBackBuffer,
	D3D12_CPU_DESCRIPTOR_HANDLE* OutBackBufferDescHeap);
typedef decltype(&QueryCurrentD3DGALDeviceContext) FuncPtr_QueryCurrentD3DGALDeviceContext;

#endif