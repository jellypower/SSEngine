#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSGAL/Public/SSGALInlineSettings.h"



#pragma region AssetManager Settings

constexpr int32 ASSETMANAGER_DEFAULT_HASHMAP_CAPACITY = 5000;
constexpr int32 CONSTANTBUFFER_DEFAULT_HASHMAP_CAPACITY = 100;


#pragma endregion


#ifdef _DEBUG
constexpr bool ENABLE_DEBUG_LAYER = true;
constexpr bool ENABLE_GPU_BASE_VALIDATIION = true;


#else
constexpr bool ENABLE_DEBUG_LAYER = false;
constexpr bool ENABLE_GPU_BASE_VALIDATIION = false;

#endif

