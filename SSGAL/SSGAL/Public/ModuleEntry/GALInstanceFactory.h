#pragma once
#include "SSGAL/ModuleExportKeyword.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"

#ifdef _WINDOWS
#include <Windows.h>

SSGAL_MODULE_NATIVE GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);
typedef decltype(&CreateGALRenderDevice) FuncPtr_CreateGALRenderDevice;
#endif

class IHasherPool;
SSGAL_MODULE_NATIVE void SSGALModuleEntry(
	IHasherPool* InHasherPool,
	IThreadManager* InThreadManager);
typedef decltype(&SSGALModuleEntry) FuncPtr_SSGALModuleEntry;