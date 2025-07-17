#include "ModuleEntryScriptRunner.h"

#include "SSBuildSettings.h"

#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"


SObjectGlobalHashMap* g_ObjectHashMap = nullptr;

IHasherPool* g_HasherPool = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;

IRenderer* g_Renderer = nullptr;

HWND g_hWnd = NULL;

HINSTANCE g_hInstSSGAL = nullptr;
HINSTANCE g_hInstSSRenderer = nullptr;
HINSTANCE g_hInstSSFBXImporter = nullptr;

FuncPtr_CreateSSFBXImporter g_fpCreateSSFBXImporter = nullptr;

void RunModuleEntryScript()
{
	g_HasherPool = CreateHasherPool(SHASHER_DEFAULT_POOL_SIZE);
	g_FrameInfoProcessor = CreateFrameInfo();
	g_RawInputProcessor = CreateInputProcessor();
	g_ObjectHashMap = CreateSObjectGlobalHashMap();


	SSEngineDefaultModuleEntry(
		SHASHER_DEFAULT_POOL_SIZE,
		g_HasherPool,
		g_FrameInfoProcessor,
		g_RawInputProcessor);

	SObjectModuleEntry(
		g_ObjectHashMap,
		g_HasherPool);

}

void RunModuleEntryScriptPostInitWindow(
	HINSTANCE hInst,
	HWND hWnd,
	bool bEnableDebugLayer,
	bool bEnableGPUBaseValidation)
{
	// Create Renderer
	{
		g_hInstSSGAL = LoadLibrary(L"SSGAL.dll");
		if (g_hInstSSGAL == nullptr)
		{
			g_hInstSSGAL = LoadLibrary(SSGAL_MODULEPATH);
		}

		g_hInstSSRenderer = LoadLibrary(L"SSRenderer.dll");
		if (g_hInstSSRenderer == nullptr)
		{
			g_hInstSSRenderer = LoadLibrary(SSRENDERER_MODULEPATH);
		}

		g_hInstSSFBXImporter = LoadLibrary(L"SSFBXImporter.dll");
		if (g_hInstSSFBXImporter == nullptr)
		{
			g_hInstSSFBXImporter = LoadLibrary(SSFBXIMPORTER_MODULEPATH);
		}

		FuncPtr_SSGALModuleEntry SSGALModuleEntry = (FuncPtr_SSGALModuleEntry)GetProcAddress(g_hInstSSGAL, "SSGALModuleEntry");
		FuncPtr_CreateGALRenderDevice CreateGALRenderDevice = (FuncPtr_CreateGALRenderDevice)GetProcAddress(g_hInstSSGAL, "CreateGALRenderDevice");

		FuncPtr_CreateRenderer CreateRenderer = (FuncPtr_CreateRenderer)GetProcAddress(g_hInstSSRenderer, "CreateRenderer");
		FuncPtr_SSRendererModuleEntry SSRendererModuleEntry = (FuncPtr_SSRendererModuleEntry)GetProcAddress(g_hInstSSRenderer, "SSRendererModuleEntry");

		FuncPtr_SSFBXImporterModuleEntry SSFBXImporterModuleEntry = (FuncPtr_SSFBXImporterModuleEntry)GetProcAddress(g_hInstSSFBXImporter, "SSFBXImporterModuleEntry");
		g_fpCreateSSFBXImporter = (FuncPtr_CreateSSFBXImporter)GetProcAddress(g_hInstSSFBXImporter, "CreateSSFBXImporter");



		SSGALModuleEntry(g_HasherPool);
		GALRenderDevice* NewRenderDevice = CreateGALRenderDevice(
			hInst,
			hWnd,
			bEnableDebugLayer,
			bEnableGPUBaseValidation);

		SSRendererModuleEntry(g_HasherPool, g_FrameInfoProcessor);
		g_Renderer = CreateRenderer(NewRenderDevice);

		SSFBXImporterModuleEntry(g_HasherPool, g_Renderer);
	}


	SSContentsBaseModuleEntry(
		g_Renderer,
		g_HasherPool,
		g_FrameInfoProcessor,
		g_RawInputProcessor);

}

void RunModuleExitScript()
{
	// Cleanup Renderer
	{
		// "g_Renderer" will be released by "SSEngine"
		g_fpCreateSSFBXImporter = nullptr;

		BOOL bSuccess = FreeLibrary(g_hInstSSFBXImporter);
		if (bSuccess == false) SS_INTERRUPT();
		bSuccess = FreeLibrary(g_hInstSSRenderer);
		if (bSuccess == false) SS_INTERRUPT();
		bSuccess = FreeLibrary(g_hInstSSGAL);
		if (bSuccess == false) SS_INTERRUPT();

		g_hInstSSFBXImporter = nullptr;
		g_hInstSSRenderer = nullptr;
		g_hInstSSGAL = nullptr;
	}

	delete g_ObjectHashMap;
	g_ObjectHashMap = nullptr;
	delete g_RawInputProcessor;
	g_RawInputProcessor = nullptr;
	delete g_FrameInfoProcessor;
	g_FrameInfoProcessor = nullptr;
	delete g_HasherPool;
	g_HasherPool = nullptr;
}
