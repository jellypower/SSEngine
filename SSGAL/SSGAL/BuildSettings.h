#pragma once

// SSEngine
#ifdef _DEBUG

#pragma comment(lib, "SSEngineDefault_Debug_x64.lib")
#pragma comment(lib, "SObject_Debug_x64.lib")

#else

#pragma comment(lib, "SSEngineDefault_Release_x64.lib")
#pragma comment(lib, "SObject_Release_x64.lib")

#endif

// DirectX
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

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
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }