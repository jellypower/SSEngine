#pragma once
#include "SSAssetDBManager/ModuleExportKeyword.h"


class IHasherPool;
class IAssetDBLoader;

SSASSETDBMANAGER_MODULE_NATIVE IAssetDBLoader* CreateAssetDBLoader();
typedef decltype(&CreateAssetDBLoader) FuncPtr_CreateAssetDBLoader;


SSASSETDBMANAGER_MODULE_NATIVE void SSAssetDBManagerModuleEntry(
	IHasherPool* InHasherPool,
	IThreadManager* InThreadManager);
typedef decltype(&SSAssetDBManagerModuleEntry) FuncPtr_SSAssetDBManagerModuleEntry;