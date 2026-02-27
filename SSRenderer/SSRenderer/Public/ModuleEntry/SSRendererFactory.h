#pragma once
#include "SSRenderer/ModuleExportKeyword.h"

class IFrameInfoProcessor;
class FrameInfoProcessorBase;
class IRenderer;
class GALRenderDevice;
class IHasherPool;
class IThreadManager;

SSRENDERER_MODULE_NATIVE IRenderer* CreateRenderer(GALRenderDevice* InRenderDevice);
typedef decltype(&CreateRenderer) FuncPtr_CreateRenderer;


SSRENDERER_MODULE_NATIVE void SSRendererModuleEntry(
	IFrameInfoProcessor* InFrameInfoProcessor,
	IThreadManager* InThreadManager);
typedef decltype(&SSRendererModuleEntry) FuncPtr_SSRendererModuleEntry;