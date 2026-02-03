#include "pch.h"
#define SSASSETDBMANAGER_MODULE_EXPORT

#include "SSAssetDBManager/Public/ModuleEntry/AssetDBLoaderFactory.h"

#include "AssetDBLoader.h"


IHasherPool* g_HasherPool = nullptr;

IAssetDBLoader* CreateAssetDBLoader()
{
	return DBG_NEW AssetDBLoader;
}

void SSAssetDBManagerModuleEntry(IHasherPool* InHasherPool)
{
	g_HasherPool = InHasherPool;
}
