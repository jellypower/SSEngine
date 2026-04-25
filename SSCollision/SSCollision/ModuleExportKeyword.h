#pragma once

#ifdef SSCOLLISION_MODULE_EXPORT
#define SSCOLLISION_MODULE __declspec(dllexport)
#define SSCOLLISION_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSCOLLISION_MODULE __declspec(dllimport)
#define SSCOLLISION_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif