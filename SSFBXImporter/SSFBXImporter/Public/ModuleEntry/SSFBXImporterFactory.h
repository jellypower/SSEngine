#pragma once
#include "SSFBXImporter/ModuleExportKeyword.h"

class IHasherPool;
class ISSFBXImporter;
class IRenderer;
class IThreadManager;
class IFrameInfoProcessor;

SSFBXIMPORTER_MODULE_NATIVE ISSFBXImporter* CreateSSFBXImporter();
typedef decltype(&CreateSSFBXImporter) FuncPtr_CreateSSFBXImporter;

SSFBXIMPORTER_MODULE_NATIVE void SSFBXImporterModuleEntry(
	IThreadManager* InThreadManager,
	IFrameInfoProcessor* InFrameInfoProcessor);
typedef decltype(&SSFBXImporterModuleEntry) FuncPtr_SSFBXImporterModuleEntry;