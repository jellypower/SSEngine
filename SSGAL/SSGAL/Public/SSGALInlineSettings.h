#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

constexpr int32 GAL_RESOURCE_DEFAULT_ALIGNMENT_SIZE = 1024 * 8 * 8; // 64KB
constexpr int32 GAL_CONSTANTBUFFER_MIN_SIZE = 256;

constexpr int32 GAL_DEFAULT_RESOURCEUUPDATER_PAGESIZE = 1024 * 1024 * 4 * 4; // 16 MB -> 작은 사이즈면 DefaultUpdateBuffer를 쪼개쓴다.
constexpr int32 GAL_INITIAL_DEFAULT_RESOURCEUPDATER_PAGECNT = 1;
constexpr int64 GAL_DEFAULT_RESOURCEUPDATE_TARGET_SIZE_MAX = 1024 * 1024 * 2 * 2; // 2 MB -> 큰 사이즈면 개별로 리소스 업데이터 버퍼를 할당해준다.

constexpr uint32 GAL_NESTED_FRAME_CNT = 2;
constexpr int32 RT_NUM_MAX = 8;