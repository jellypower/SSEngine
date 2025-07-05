#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

class IRenderer;
class GALRenderDevice;

SSRENDERER_MODULE_NATIVE IRenderer* CreateRenderer(GALRenderDevice* InRenderDevice);

typedef decltype(&CreateRenderer) FuncPtr_CreateRender;