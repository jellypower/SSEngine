#pragma once
#include <Windows.h>


#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"


class SSRenderer;

GALRenderDevice* CreateGALRenderDevice(
	HINSTANCE InhInst,
	HWND InhWnd,
	bool EnableDebugLayer,
	bool EnableGPUBaseValidataion);
