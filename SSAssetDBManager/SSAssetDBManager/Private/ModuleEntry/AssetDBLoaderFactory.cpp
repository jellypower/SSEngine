#include "pch.h"
#define SSASSETDBMANAGER_MODULE_EXPORT

#include "SSAssetDBManager/Public/ModuleEntry/AssetDBLoaderFactory.h"

#include "SSAssetDBManager/Private/DBLoaders/AssetDBLoader.h"


IThreadManager* g_ThreadManager = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IAssetDBLoader* CreateAssetDBLoader()
{
	return DBG_NEW AssetDBLoader;
}

void SSAssetDBManagerModuleEntry(
	IThreadManager* InThreadManager,
	IFrameInfoProcessor* InFrameInfoProcessor)
{
	g_ThreadManager = InThreadManager;
	g_FrameInfoProcessor = InFrameInfoProcessor;
}
