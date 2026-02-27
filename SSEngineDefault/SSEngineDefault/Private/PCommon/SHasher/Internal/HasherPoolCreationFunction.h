#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"


class IHasherPool;
IHasherPool* CreateHasherPool(int32 InBucketCnt);
