#pragma once

#ifdef SSFBXIMPORTER_MODULE_EXPORT
#define SSFBXIMPORTER_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSFBXIMPORTER_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif
