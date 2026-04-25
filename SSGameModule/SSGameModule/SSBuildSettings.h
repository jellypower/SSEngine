#pragma once


// SSEditor
#ifdef _DEBUG

#pragma comment(lib, "SSEngineDefault_Debug_x64.lib")
#pragma comment(lib, "SObject_Debug_x64.lib")
#pragma comment(lib, "SSContentsBase_Debug_x64.lib")
#pragma comment(lib, "SSRenderer_Debug_x64.lib")

#define SSGAL_MODULE_NAME L"SSGAL_Debug_x64.dll"
#define SSRENDERER_MODULE_NAME L"SSRenderer_Debug_x64.dll"
#define SSCOLLISION_MODULEPATH L"SSCollision_Debug_x64.dll"
#define SSFBXIMPORTER_MODULE_NAME L"SSFBXImporter_Debug_x64.dll"
#define SSASSETDBMANAGER_MODULE_NAME L"SSAssetDBManager_Debug_x64.dll"

#define SSGAL_MODULEPATH L"../x64/Debug/SSGAL_Debug_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Debug/SSRenderer_Debug_x64.dll"
#define SSCOLLISION_MODULEPATH L"../x64/Debug/SSCollision_Debug_x64.dll"
#define SSFBXIMPORTER_MODULEPATH L"../x64/Debug/SSFBXImporter_Debug_x64.dll"
#define SSASSETDBMANAGER_MODULEPATH L"../x64/Debug/SSAssetDBManager_Debug_x64.dll"

#else

#pragma comment(lib, "SSEngineDefault_Release_x64.lib")
#pragma comment(lib, "SObject_Release_x64.lib")
#pragma comment(lib, "SSContentsBase_Release_x64.lib")
#pragma comment(lib, "SSRenderer_Release_x64.lib")

#define SSGAL_MODULE_NAME L"SSGAL_Release_x64.dll"
#define SSRENDERER_MODULE_NAME L"SSRenderer_Release_x64.dll"
#define SSCOLLISION_MODULEPATH L"SSCollision_Release_x64.dll"
#define SSFBXIMPORTER_MODULE_NAME L"SSFBXImporter_Release_x64.dll"
#define SSASSETDBMANAGER_MODULE_NAME L"SSAssetDBManager_Release_x64.dll"

#define SSGAL_MODULEPATH L"../x64/Release/SSGAL_Release_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Release/SSRenderer_Release_x64.dll"
#define SSCOLLISION_MODULEPATH L"../x64/Release/SSCollision_Release_x64.dll"
#define SSFBXIMPORTER_MODULEPATH L"../x64/Release/SSFBXImporter_Release_x64.dll"
#define SSASSETDBMANAGER_MODULEPATH L"../x64/Release/SSAssetDBManager_Release_x64.dll"

#endif

