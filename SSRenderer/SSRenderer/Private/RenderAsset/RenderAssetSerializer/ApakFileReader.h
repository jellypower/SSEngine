#pragma once
#include "SSRenderer/Public/RenderAssetSerializer/ApakDataChunkOffsetDesc.h"
#include "SSRenderer/Public/RenderAssetSerializer/IApakFileReader.h"

class ApakFileReader : public IApakFileReader
{
public:
	ApakFileReader(SS::SHasherW InFilePath, SS::SHasherW TargetDBNameSpace);
	virtual ~ApakFileReader() override;

public:
	virtual SS::SHasherW GetApakAssetName(int32 Idx) const override;

	virtual SS::SHasherW GetFilePath() override;
	virtual int32 GetApakAssetCnt() const override;
	virtual bool IsValid() const override;

	virtual const SS::PooledList<byte>& GetCursoredData() const override;

public:
	virtual bool SetDataCursorToAsset(SS::SHasherW AssetName) override;

private:
	void HandleFailOnLoad();

private:
	SS::SHasherW _TargetDBNameSpace;
	SS::SHasherW _FilePath;
	FILE* _hFile = nullptr;
	SS::PooledList<byte> _FReadData;

	ApakFileHeader _FileHeader;

private:
	SS::PooledList<SS::SHasherW> _AssetNames;
	SS::PooledList<ApakDataChunkOffsetDesc> _AssetOffsets;
};
