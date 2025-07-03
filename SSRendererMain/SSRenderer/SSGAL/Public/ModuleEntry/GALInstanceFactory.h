#pragma once
#include <Windows.h>


#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"



GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);
