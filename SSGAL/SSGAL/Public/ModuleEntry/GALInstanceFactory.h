#pragma once
#include "SSGAL/ModuleExportKeyword.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"

#include <Windows.h>


class IHasherPool;

SSGAL_MODULE_NATIVE GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);

typedef decltype(&CreateGALRenderDevice) FuncPtr_CreateGALRenderDevice;


SSGAL_MODULE_NATIVE void SSGALModuleEntry(IHasherPool* InHasherPool);

typedef decltype(&SSGALModuleEntry) FuncPtr_SSGALModuleEntry;
