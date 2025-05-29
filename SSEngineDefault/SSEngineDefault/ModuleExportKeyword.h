#pragma once

#ifdef SSENGINEDEFAULT_MODULE_EXPORT
#define SSENGINEDEFAULT_MODULE __declspec(dllexport)
#define SSENGINEDEFAULT_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSENGINEDEFAULT_MODULE __declspec(dllimport)
#define SSENGINEDEFAULT_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif