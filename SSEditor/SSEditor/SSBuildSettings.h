#pragma once


#pragma comment(lib, "DXGI.lib") // ImGUI에서 DXGI초기화가 필요해서 추가

#ifdef _DEBUG

#pragma comment(lib, "SSEngineDefault_Debug_x64.lib")
#pragma comment(lib, "SObject_Debug_x64.lib")
#pragma comment(lib, "SSContentsBase_Debug_x64.lib")
#pragma comment(lib, "SSRenderer_Debug_x64.lib")
#pragma comment(lib, "SSImgui_Debug_x64.lib")
#pragma comment(lib, "SSGameModule_Debug_x64.lib")


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
#pragma comment(lib, "SSImgui_Release_x64.lib")
#pragma comment(lib, "SSGameModule_Release_x64.lib")

#define SSGAL_MODULEPATH L"../x64/Release/SSGAL_Release_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Release/SSRenderer_Release_x64.dll"
#define SSCOLLISION_MODULEPATH L"../x64/Release/SSCollision_Release_x64.dll"
#define SSFBXIMPORTER_MODULEPATH L"../x64/Release/SSFBXImporter_Release_x64.dll"
#define SSASSETDBMANAGER_MODULEPATH L"../x64/Release/SSAssetDBManager_Release_x64.dll"

#endif

