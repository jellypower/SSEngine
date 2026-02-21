#include "pch.h"
#define SSASSETDBMANAGER_MODULE_EXPORT

#include "SSAssetDBManager/Public/ModuleEntry/AssetDBLoaderFactory.h"

#include "SSAssetDBManager/Private/DBLoaders/AssetDBLoader.h"


IHasherPool* g_HasherPool = nullptr;
IThreadManager* g_ThreadManager = nullptr;

IAssetDBLoader* CreateAssetDBLoader()
{
	return DBG_NEW AssetDBLoader;
}

void SSAssetDBManagerModuleEntry(
	IHasherPool* InHasherPool,
	IThreadManager* InThreadManager)
{
	g_HasherPool = InHasherPool;
	g_ThreadManager = InThreadManager;
}
