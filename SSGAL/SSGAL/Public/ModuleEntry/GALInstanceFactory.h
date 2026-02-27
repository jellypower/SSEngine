#pragma once
#include "SSGAL/ModuleExportKeyword.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"

#ifdef _WINDOWS
#include <Windows.h>


SSGAL_MODULE_NATIVE GALRenderTarget* CreateGALSwapChain(
	GALRenderDeviceContext* DeviceContext,
	HWND InhWnd);
typedef decltype(&CreateGALSwapChain) FuncPtr_CreateGALSwapChain;

#endif


class IHasherPool;

SSGAL_MODULE_NATIVE GALRenderDevice* CreateGALRenderDevice(
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);
typedef decltype(&CreateGALRenderDevice) FuncPtr_CreateGALRenderDevice;

SSGAL_MODULE_NATIVE void SSGALModuleEntry(
	IFrameInfoProcessor* InFrameInfoProcessor,
	IThreadManager* InThreadManager);
typedef decltype(&SSGALModuleEntry) FuncPtr_SSGALModuleEntry;