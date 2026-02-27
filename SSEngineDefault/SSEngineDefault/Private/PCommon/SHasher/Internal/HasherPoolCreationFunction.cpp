#include "HasherPoolCreationFunction.h"

#include "SSEngineDefault/Private/PCommon/SHasher/HasherPoolBase.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"


IHasherPool* CreateHasherPool(int32 InBucketCnt)
{
	HasherPoolBase* NewHasherPool = DBG_NEW HasherPoolBase(InBucketCnt);
	return NewHasherPool;
}
