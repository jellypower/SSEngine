#pragma once
#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderer;

class IHasherPool;
class FrameInfoProcessorBase;
class SSRawInputProcessorBase;

SSCONTENTBASE_MODULE_NATIVE void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	IHasherPool* InHasherPool,
	FrameInfoProcessorBase* InFrameInfo,
	SSRawInputProcessorBase* InRawInputProcessor);
