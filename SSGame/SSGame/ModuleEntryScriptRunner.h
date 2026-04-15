#pragma once


#include "SSAssetDBManager/Public/ModuleEntry/AssetDBLoaderFactory.h"

extern HINSTANCE g_hInstSSGAL;
extern HINSTANCE g_hInstSSRenderer;
extern HINSTANCE g_hInstSSFBXImporter;
extern HINSTANCE g_hInstSSAssetDBManager;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern RECT g_WndRect;


extern FuncPtr_CreateAssetDBLoader g_fpCreateAssetDBLoader;

void RunLoadLibraries();
void RunModuleEntryScript();
void RunModuleEntryScriptPostInitWindow(
	HINSTANCE hInst,
	HWND hWnd,
	bool bEnableDebugLayer,
	bool bEnableGPUBaseValidation);


void RunModuleExitScript();
void RunUnloadLibraries();