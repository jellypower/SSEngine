#pragma once

#ifdef SSGAL_MODULE_EXPORT
#define SSGAL_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSGAL_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif