#include "pch.h"

#include "ModuleEntryScriptRunner.h"

#include <SSEngineDefault/Public/WindowManager/WindowUtils.h>

#include "SSBuildSettings.h"

#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/SSThread/IThreadManager.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"
#include "SSEngineDefault/Public/SSThread/PWin32/SSThreadUtil_Win32.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"
#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"


// SObjectGlobalVariableSet
SObjectGlobalHashMap* g_ObjectHashMap = nullptr;
// ~SObjectGlobalVariableSet


// GlobalVariableSet
IWindowManager* g_MainWindowManager = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;
// ~GlobalVariableSet

// SSRendererGlobalVariableSet
IRenderer* g_Renderer = nullptr;
// ~SSRendererGlobalVariableSet

HWND g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
RECT g_WndRect = { 0,0,1920,1080 };

HINSTANCE g_hInstSSGAL = nullptr;
HINSTANCE g_hInstSSRenderer = nullptr;
HINSTANCE g_hInstSSFBXImporter = nullptr;
HINSTANCE g_hInstSSAssetDBManager = nullptr;

FuncPtr_CreateAssetDBLoader g_fpCreateAssetDBLoader = nullptr;



void RunLoadLibraries()
{
	g_hInstSSGAL = LoadLibrary(SSGAL_MODULE_NAME);
	g_hInstSSRenderer = LoadLibrary(SSRENDERER_MODULE_NAME);
	g_hInstSSFBXImporter = LoadLibrary(SSFBXIMPORTER_MODULE_NAME);
	g_hInstSSAssetDBManager = LoadLibrary(SSASSETDBMANAGER_MODULE_NAME);

	if (g_hInstSSGAL == nullptr) g_hInstSSGAL = LoadLibrary(SSGAL_MODULEPATH);
	if (g_hInstSSRenderer == nullptr) g_hInstSSRenderer = LoadLibrary(SSRENDERER_MODULEPATH);
	if (g_hInstSSFBXImporter == nullptr) g_hInstSSFBXImporter = LoadLibrary(SSFBXIMPORTER_MODULEPATH);
	if (g_hInstSSAssetDBManager == nullptr) g_hInstSSAssetDBManager = LoadLibrary(SSASSETDBMANAGER_MODULEPATH);
}

void RunModuleEntryScript()
{
	g_MainWindowManager = CreateWindowManager();
	g_FrameInfoProcessor = CreateFrameInfo();
	g_RawInputProcessor = CreateInputProcessor();
	g_ObjectHashMap = CreateSObjectGlobalHashMap();
	g_ThreadManager = CreateThreadManager();

	HANDLE MainThread = ::GetCurrentThread();
	SetMainThreadHandle(g_ThreadManager, MainThread);
	SS_ASSERT(g_ThreadManager->IsInMainThread());

	SSEngineDefaultModuleEntry(
		g_MainWindowManager,
		g_FrameInfoProcessor,
		g_RawInputProcessor,
		g_ThreadManager);

	SObjectModuleEntry(
		g_ObjectHashMap,
		g_ThreadManager);

}

void RunModuleEntryScriptPostInitWindow(
	HINSTANCE hInst,
	HWND hWnd,
	bool bEnableDebugLayer,
	bool bEnableGPUBaseValidation)
{
	InitWindowManagerWinHandle(g_MainWindowManager, g_hWnd, g_WndRect, g_hInst);

	// Create Renderer
	{
		FuncPtr_SSGALModuleEntry SSGALModuleEntry = (FuncPtr_SSGALModuleEntry)GetProcAddress(g_hInstSSGAL, "SSGALModuleEntry");
		FuncPtr_CreateGALRenderDevice CreateGALRenderDevice = (FuncPtr_CreateGALRenderDevice)GetProcAddress(g_hInstSSGAL, "CreateGALRenderDevice");
		FuncPtr_CreateGALSwapChain CreateGALSwapChain = (FuncPtr_CreateGALSwapChain)GetProcAddress(g_hInstSSGAL, "CreateGALSwapChain");


		FuncPtr_CreateRenderer CreateRenderer = (FuncPtr_CreateRenderer)GetProcAddress(g_hInstSSRenderer, "CreateRenderer");
		FuncPtr_SSRendererModuleEntry SSRendererModuleEntry = (FuncPtr_SSRendererModuleEntry)GetProcAddress(g_hInstSSRenderer, "SSRendererModuleEntry");

		
		FuncPtr_SSAssetDBManagerModuleEntry SSAssetDBManagerModuleEntry = (FuncPtr_SSAssetDBManagerModuleEntry)GetProcAddress(g_hInstSSAssetDBManager, "SSAssetDBManagerModuleEntry");
		g_fpCreateAssetDBLoader = (FuncPtr_CreateAssetDBLoader)GetProcAddress(g_hInstSSAssetDBManager, "CreateAssetDBLoader");

		SSGALModuleEntry(
			g_FrameInfoProcessor,
			g_ThreadManager);

		GALRenderDevice* NewRenderDevice = CreateGALRenderDevice(
			bEnableDebugLayer,
			bEnableGPUBaseValidation);

		SSRendererModuleEntry(
			g_FrameInfoProcessor,
			g_ThreadManager);
		g_Renderer = CreateRenderer(NewRenderDevice);


		GALRenderTarget* SwapChainRenderTarget = CreateGALSwapChain(g_Renderer->GetMainDeviceContext(), hWnd);
		g_Renderer->HandoverMainViewportSwapChain(SwapChainRenderTarget);


		SSAssetDBManagerModuleEntry(
			g_ThreadManager,
			g_FrameInfoProcessor);
	}


	SSContentsBaseModuleEntry(
		g_Renderer,
		g_FrameInfoProcessor,
		g_RawInputProcessor,
		g_ThreadManager);

}

void RunModuleExitScript()
{
	// Cleanup Renderer
	delete g_ThreadManager;
	g_ThreadManager = nullptr;
	delete g_ObjectHashMap;
	g_ObjectHashMap = nullptr;
	delete g_RawInputProcessor;
	g_RawInputProcessor = nullptr;
	delete g_FrameInfoProcessor;
	g_FrameInfoProcessor = nullptr;
	delete g_MainWindowManager;
	g_MainWindowManager = nullptr;

	DestroyGlobalHasherPool();
}

void RunUnloadLibraries()
{
	BOOL bSuccess = FreeLibrary(g_hInstSSFBXImporter);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSRenderer);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSGAL);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSAssetDBManager);
	if (bSuccess == false) SS_INTERRUPT();

	g_hInstSSFBXImporter = nullptr;
	g_hInstSSRenderer = nullptr;
	g_hInstSSGAL = nullptr;
}
