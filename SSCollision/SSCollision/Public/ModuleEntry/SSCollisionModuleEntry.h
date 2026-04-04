#pragma once
#include "SSCollision/ModuleExportKeyword.h"

class IRenderer;
class IFrameInfoProcessor;
class IThreadManager;


SSCOLLISION_MODULE_NATIVE void SSCollisionModuleEntry(
	IFrameInfoProcessor* InFrameInfo,
	IThreadManager* InThreadManager);

typedef decltype(&SSCollisionModuleEntry) FuncPtr_SSCollisionModuleEntry;