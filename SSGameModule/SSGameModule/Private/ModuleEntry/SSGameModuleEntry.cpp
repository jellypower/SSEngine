#include "pch.h"
#define SSGAME_MODULE_EXPORT
#include "SSGameModule/Public/ModuleEntry/SSGameModuleEntry.h"


#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"



// SSRendererGlobalVariableSet
IRenderer* g_Renderer = nullptr;
// ~SSRendererGlobalVariableSet

// SSCollisionGlobalVariableSet
ICollDevice* g_CollDevice = nullptr;
// ~SSCollisionGlobalVariableSet

// GlobalVariableSet
IWindowManager* g_MainWindowManager = nullptr;
IFrameInfoProcessor* g_FrameInfoProcessor = nullptr;
IRawInputProcessor* g_RawInputProcessor = nullptr;
IThreadManager* g_ThreadManager = nullptr;
// ~GlobalVariableSet


void SSGameModuleEntry(
	IRenderer* InRenderer, 
	ICollDevice* InCollDevice, 
	IFrameInfoProcessor* InFrameInfo,
	IRawInputProcessor* InRawInputProcessor, 
	IThreadManager* InThreadManager, 
	IWindowManager* InMainWindowManager)
{
	g_Renderer = InRenderer;

	g_CollDevice = InCollDevice;

	g_FrameInfoProcessor = InFrameInfo;
	g_RawInputProcessor = InRawInputProcessor;
	g_ThreadManager = InThreadManager;
	g_MainWindowManager = InMainWindowManager;
}
