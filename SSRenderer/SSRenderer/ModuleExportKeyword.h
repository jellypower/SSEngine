#pragma once

#ifdef SSRENDERER_MODULE_EXPORT
#define SSRENDERER_MODULE __declspec(dllexport)
#define SSRENDERER_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSRENDERER_MODULE __declspec(dllimport)
#define SSRENDERER_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif