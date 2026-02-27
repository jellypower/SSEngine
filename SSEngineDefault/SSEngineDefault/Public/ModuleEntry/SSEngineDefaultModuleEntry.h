#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

class IThreadManager;
class IFrameInfoProcessor;
class IHasherPool;
class IRawInputProcessor;



SSENGINEDEFAULT_MODULE_NATIVE void SSEngineDefaultModuleEntry(
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager);

SSENGINEDEFAULT_MODULE_NATIVE IRawInputProcessor* CreateInputProcessor();
SSENGINEDEFAULT_MODULE_NATIVE IFrameInfoProcessor* CreateFrameInfo();
SSENGINEDEFAULT_MODULE_NATIVE IThreadManager* CreateThreadManager();