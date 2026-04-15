#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


struct ApakDataChunkOffsetDesc;

class IApakFileReader : public INoncopyable
{
public:
	virtual SS::SHasherW GetApakAssetName(int32 Idx) const = 0;

	virtual SS::SHasherW GetFilePath() = 0;
	virtual int32 GetApakAssetCnt() const = 0;
	virtual bool IsValid() const = 0;

	virtual const SS::PooledList<byte>& GetCursoredData() const = 0;

public:
	virtual bool SetDataCursorToAsset(SS::SHasherW AssetName) = 0;
};
