#pragma once

#ifdef DLL_EXPORT
#define SSENGINEDEFAULT_MODULE __declspec(dllexport)
#else
#define SSENGINEDEFAULT_MODULE __declspec(dllimport)
#endif