#pragma once

#ifdef SSCONTENTBASE_MODULE_EXPORT
#define SSCONTENTBASE_MODULE __declspec(dllexport)
#define SSCONTENTBASE_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSCONTENTBASE_MODULE __declspec(dllexport)
#define SSCONTENTBASE_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif