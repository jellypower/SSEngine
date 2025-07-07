#pragma once
#include "SSFBXImporter/ModuleExportKeyword.h"

class IHasherPool;
class ISSFBXImporter;
class IRenderer;


SSFBXIMPORTER_MODULE_NATIVE ISSFBXImporter* CreateSSFBXImporter();
typedef decltype(&CreateSSFBXImporter) FuncPtr_CreateSSFBXImporter;

SSFBXIMPORTER_MODULE_NATIVE void SSFBXImporterModuleEntry(IHasherPool* InHasherPool, IRenderer* InRenderer);
typedef decltype(&SSFBXImporterModuleEntry) FuncPtr_SSFBXImporterModuleEntry;