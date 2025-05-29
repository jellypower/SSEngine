#pragma once

#ifdef SOBJECT_MODULE_EXPORT
#define SOBJECT_MODULE __declspec(dllexport)
#define SOBJECT_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SOBJECT_MODULE __declspec(dllimport)
#define SOBJECT_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif