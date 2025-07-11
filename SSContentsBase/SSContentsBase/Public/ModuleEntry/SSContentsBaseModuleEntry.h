#pragma once
#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderer;

class IHasherPool;
class IFrameInfoProcessor;
class SSRawInputProcessorBase;

SSCONTENTBASE_MODULE_NATIVE void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	IHasherPool* InHasherPool,
	IFrameInfoProcessor* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);
