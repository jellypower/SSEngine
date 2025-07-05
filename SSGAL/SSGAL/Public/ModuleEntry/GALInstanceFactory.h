#pragma once
#include "SSGAL/ModuleExportKeyword.h"
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"


#include <Windows.h>

SSGAL_MODULE_NATIVE GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);
