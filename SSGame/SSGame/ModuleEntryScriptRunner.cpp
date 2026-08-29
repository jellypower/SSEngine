#include "pch.h"

#include "ModuleEntryScriptRunner.h"

#include "SSBuildSettings.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/SSThread/IThreadManager.h"
#include "SSEngineDefault/Public/SSThread/PWin32/SSThreadUtil_Win32.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"
#include "SSEngineDefault/Public/WindowManager/WindowUtils.h"

#include "SObject/Public/SObjectGlobalHashMap.h"
#include "SObject/Public/GlobalVariableSet/SObjectGlobalVariableSet.h"
#include "SObject/Public/ModuleEntry/SObjectModuleEntry.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSRenderer/Public/ModuleEntry/SSRendererFactory.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"

#include "SSCollision/Public/ModuleEntry/CollisionWorldFactory.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionModuleEntry.h"

#include "SSContentsBase/Public/ModuleEntry/SSContentsBaseModuleEntry.h"


#include "SSGameModule/Public/ModuleEntry/SSGameModuleEntry.h"


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

// SSCollisionGlobalVariableSet
ICollDevice* g_CollDevice = nullptr;
// ~SSCollisionGlobalVariableSet

HWND g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
RECT g_WndRect = { 0,0,1920,1080 };

HINSTANCE g_hInstSSGAL = nullptr;
HINSTANCE g_hInstSSRenderer = nullptr;
HINSTANCE g_hInstSSCollision = nullptr;
HINSTANCE g_hInstSSAssetDBManager = nullptr;

FuncPtr_CreateAssetDBLoader g_fpCreateAssetDBLoader = nullptr;



void RunLoadLibraries()
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

	g_hInstSSCollision = LoadLibrary(L"SSCollision.dll");
	if (g_hInstSSCollision == nullptr)
	{
		g_hInstSSCollision = LoadLibrary(SSCOLLISION_MODULEPATH);
	}

	g_hInstSSAssetDBManager = LoadLibrary(L"SSAssetDBManager.dll");
	if (g_hInstSSAssetDBManager == nullptr)
	{
		g_hInstSSAssetDBManager = LoadLibrary(SSASSETDBMANAGER_MODULEPATH);
	}
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

		FuncPtr_CreateCollDevice CreateCollDevice = (FuncPtr_CreateCollDevice)GetProcAddress(g_hInstSSCollision, "CreateCollDevice");
		FuncPtr_SSCollisionModuleEntry SSCollisionModuleEntry = (FuncPtr_SSCollisionModuleEntry)GetProcAddress(g_hInstSSCollision, "SSCollisionModuleEntry");

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


		g_CollDevice = CreateCollDevice();
		SSCollisionModuleEntry(
			g_FrameInfoProcessor,
			g_ThreadManager);


		GALRenderTarget* SwapChainRenderTarget = CreateGALSwapChain(g_Renderer->GetMainDeviceContext(), hWnd);
		g_Renderer->HandoverMainViewportSwapChain(SwapChainRenderTarget);

		SSAssetDBManagerModuleEntry(
			g_ThreadManager,
			g_FrameInfoProcessor);
	}


	SSContentsBaseModuleEntry(
		g_Renderer,
		g_CollDevice,
		g_FrameInfoProcessor,
		g_RawInputProcessor,
		g_ThreadManager);

	SSGameModuleEntry(
		g_Renderer,
		g_CollDevice,
		g_FrameInfoProcessor,
		g_RawInputProcessor,
		g_ThreadManager,
		g_MainWindowManager
	);

}

void RunModuleExitScript()
{
	// Cleanup Renderer
	g_ThreadManager->Release();
	g_ThreadManager = nullptr;
	delete g_ObjectHashMap;
	g_ObjectHashMap = nullptr;
	g_RawInputProcessor->Release();
	g_RawInputProcessor = nullptr;
	g_FrameInfoProcessor->Release();
	g_FrameInfoProcessor = nullptr;
	g_MainWindowManager->Release();
	g_MainWindowManager = nullptr;

	DestroyGlobalHasherPool();
}

void RunUnloadLibraries()
{
	BOOL bSuccess = FreeLibrary(g_hInstSSCollision);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSRenderer);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSGAL);
	if (bSuccess == false) SS_INTERRUPT();
	bSuccess = FreeLibrary(g_hInstSSAssetDBManager);
	if (bSuccess == false) SS_INTERRUPT();

	g_hInstSSRenderer = nullptr;
	g_hInstSSGAL = nullptr;
}
