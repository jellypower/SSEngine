#pragma once
#include <Windows.h>


// FBXSDK
#pragma comment(lib, "libfbxsdk.lib")

// SSEngine
#pragma comment(lib, "SSEngineDefault.lib")
#pragma comment(lib, "SObject.lib")



#ifdef _DEBUG

#define SSGAL_MODULEPATH L"../x64/Debug/SSGAL_Debug_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Debug/SSRenderer_Debug_x64.dll"

#else

#define SSGAL_MODULEPATH L"../x64/Release/SSGAL_Release_x64.dll"
#define SSRENDERER_MODULEPATH L"../x64/Release/SSRenderer_Release_x64.dll"

#endif

