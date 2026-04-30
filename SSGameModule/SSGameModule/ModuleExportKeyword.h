#pragma once


#ifdef SSGAME_MODULE_EXPORT
#define SSGAME_MODULE __declspec(dllexport)
#define SSGAME_MODULE_NATIVE extern "C" __declspec(dllexport)
#else
#define SSGAME_MODULE __declspec(dllexport)
#define SSGAME_MODULE_NATIVE extern "C" __declspec(dllimport)
#endif