#pragma once
#include <Windows.h>

#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"


extern HINSTANCE g_hInstSSGAL;
extern HINSTANCE g_hInstSSRenderer;
extern HINSTANCE g_hInstSSFBXImporter;
extern HWND g_hWnd;


extern FuncPtr_CreateSSFBXImporter g_fpCreateSSFBXImporter;


void RunModuleEntryScript();

void RunModuleEntryScriptPostInitWindow(
	HINSTANCE hInst,
	HWND hWnd,
	bool bEnableDebugLayer,
	bool bEnableGPUBaseValidation);
void RunModuleExitScript();