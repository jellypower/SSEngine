#pragma once


// SSEditor
#pragma comment(lib, "SSEngineDefault.lib")
#pragma comment(lib, "SObject.lib")
#pragma comment(lib, "SSContentsBase.lib")
#pragma comment(lib, "SSRenderer.lib")


#ifdef _DEBUG


#define SSGAL_MODULE_NAME L"SSGAL_Debug_x64.dll"
#define SSRENDERER_MODULE_NAME L"SSRenderer_Debug_x64.dll"
#define SSFBXIMPORTER_MODULE_NAME L"SSFBXImporter_Debug_x64.dll"
#define SSASSETDBMANAGER_MODULE_NAME L"SSAssetDBManager_Debug_x64.dll"

#define SSGAL_MODULEPATH L"../x64/Debug/SSGAL_Debug_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Debug/SSRenderer_Debug_x64.dll"
#define SSFBXIMPORTER_MODULEPATH L"../x64/Debug/SSFBXImporter_Debug_x64.dll"
#define SSASSETDBMANAGER_MODULEPATH L"../x64/Debug/SSAssetDBManager_Debug_x64.dll"

#else

#define SSGAL_MODULE_NAME L"SSGAL_Release_x64.dll"
#define SSRENDERER_MODULE_NAME L"SSRenderer_Release_x64.dll"
#define SSFBXIMPORTER_MODULE_NAME L"SSFBXImporter_Release_x64.dll"
#define SSASSETDBMANAGER_MODULE_NAME L"SSAssetDBManager_Release_x64.dll"

#define SSGAL_MODULEPATH L"../x64/Release/SSGAL_Release_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Release/SSRenderer_Release_x64.dll"
#define SSFBXIMPORTER_MODULEPATH L"../x64/Release/SSFBXImporter_Release_x64.dll"
#define SSASSETDBMANAGER_MODULEPATH L"../x64/Release/SSAssetDBManager_Release_x64.dll"

#endif

