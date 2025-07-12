#pragma once
#include "SSContentsBase/ModuleExportKeyword.h"

class IRenderer;

class IHasherPool;
class IFrameInfoProcessor;
class IRawInputProcessor;

SSCONTENTBASE_MODULE_NATIVE void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	IHasherPool* InHasherPool,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor);
