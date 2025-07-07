#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

class IRenderer;
class GALRenderDevice;
class IHasherPool;

SSRENDERER_MODULE_NATIVE IRenderer* CreateRenderer(GALRenderDevice* InRenderDevice);

typedef decltype(&CreateRenderer) FuncPtr_CreateRenderer;


SSRENDERER_MODULE_NATIVE void SSRendererModuleEntry(IHasherPool* InHasherPool);

typedef decltype(&SSRendererModuleEntry) FuncPtr_SSRendererModuleEntry;