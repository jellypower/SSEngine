#pragma once
#include "SSContentsBase/ModuleExportKeyword.h"


class IRenderer;

class ICollDevice;

class IHasherPool;
class IFrameInfoProcessor;
class IRawInputProcessor;
class IThreadManager;

SSCONTENTBASE_MODULE_NATIVE void SSContentsBaseModuleEntry(
	IRenderer* InRenderer,
	ICollDevice* InCollDevice,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager);
