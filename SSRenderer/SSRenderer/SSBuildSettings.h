#pragma once
#include <Windows.h>

// required .lib files
// 추가 종속성을 의미하는듯
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// FBXSDK
#pragma comment(lib, "libfbxsdk.lib")

// SSEngine
#pragma comment(lib, "SSEngineDefault.lib")


// __declspec(dllexport) 을 사용하면 외부 dll에서도 해당 변수의 값을 알 수 있음
// 이를 통해 DX12 Agility SDK에 사용할 D3DSDKPath를 알려줌
#if defined(_M_ARM64EC)
extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\arm64\\"; }
#elif defined(_M_ARM64)
extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\arm64\\"; }
#elif defined(_M_AMD64)
extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\x64\\"; }
#elif defined(_M_IX86)
extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\x86\\"; }
#endif


extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

//////////////////////////////////////////////////////////////////////////////////////////////////////
// D3D12 Agility SDK Runtime

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }