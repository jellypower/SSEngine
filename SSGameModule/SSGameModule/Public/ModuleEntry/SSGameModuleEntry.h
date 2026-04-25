#pragma once
#include <SSEngineDefault/Public/WindowManager/IWindowManager.h>

#include "SSGameModule/ModuleExportKeyword.h"


class IRenderer;

class ICollDevice;

class IHasherPool;
class IFrameInfoProcessor;
class IRawInputProcessor;
class IThreadManager;


SSGAME_MODULE_NATIVE void SSGameModuleEntry(
	IRenderer* InRenderer,
	ICollDevice* InCollDevice,
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor,
	IThreadManager* InThreadManager,
	IWindowManager* InMainWindowManager);