#pragma once


#ifdef SSASSETDBMANAGER_MODULE_EXPORT
#define SSASSETDBMANAGER_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSASSETDBMANAGER_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif