#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"


struct TransientChunkHeader
{
	void* PageContent = nullptr;
	int32 ChunkOffset = 0;
	int32 Size = 0;
};
