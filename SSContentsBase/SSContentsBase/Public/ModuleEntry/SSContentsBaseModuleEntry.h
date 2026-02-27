#pragma once
#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderer;

class IHasherPool;
class IFrameInfoProcessor;
class IRawInputProcessor;
class IThreadManager;

SSCONTENTBASE_MODULE_NATIVE void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager);
