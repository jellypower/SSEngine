#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


struct ApakFileHeader
{
	int64 FileTotalBytes;
	int64 SerializableAssetCnt;
	int64 Padd1;
	int64 Padd2;
};


// Alloc Serialize Data Chunk offset
struct ApakDataChunkOffsetDesc
{
	int64 Offset;
	int64 Size;
};