#include "ApakFileReader.h"

#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"

ApakFileReader::ApakFileReader(SS::SHasherW InFilePath, SS::SHasherW TargetDBNameSpace)
{
	// fopen해보고 파일 여는걸 실패하면 ApakFileAccessor자체를 만들어주지 않을수도 있기 때문에 주입해주는 형태
	_FilePath = InFilePath;
	_TargetDBNameSpace = TargetDBNameSpace;

	errno_t no = _wfopen_s(&_hFile, _FilePath.C_Str(), L"rb");
	if (no != 0)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}


	constexpr int32 TEMP_READ_DATA_RESERVE_SIZE = 1024;
	_FReadData.Reserve(TEMP_READ_DATA_RESERVE_SIZE);

	constexpr int32 FILE_INIT_READ_SIZE = sizeof(ApakFileHeader) + sizeof(int32);
	_FReadData.SetSizeDirectly(FILE_INIT_READ_SIZE);
	// ApakFileHeader + PreLoadStringTableBytes

	size_t ReadSize = fread_s(
		_FReadData.GetData(),
		_FReadData.GetSize(),
		1,
		FILE_INIT_READ_SIZE,
		_hFile);
	if (ReadSize != FILE_INIT_READ_SIZE)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}


	int32 Offset = 0;
	Offset += FillMemoryFromData(&_FileHeader, sizeof(ApakFileHeader), _FReadData, Offset);
	if (_FileHeader.SerializableAssetCnt <= 0)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}

	
	int32 PreLoadStringTableBytes;
	int32 ReadBytes = FillMemoryFromData(&PreLoadStringTableBytes, sizeof(int32), _FReadData, Offset);
	if (ReadBytes != sizeof(int32) ||
		PreLoadStringTableBytes <= 0)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}

	const int64 OffsetTableBytes = sizeof(ApakDataChunkOffsetDesc) * _FileHeader.SerializableAssetCnt;
	const int64 NewSize = OffsetTableBytes + (int64)PreLoadStringTableBytes;

	// 이전 데이터에다가 추후 읽어올 PreLoadStringTableBytes 만큼을 더 읽어온다.
	// (PreLoadStringTableBytes는 이미 스트링테이블의 전체 사이즈인 int32데이터를 포함한 크기이다.)
	_FReadData.SetSizeDirectly(Offset + NewSize);

	constexpr int32 REWIND_SIZE = -1 * sizeof(int32);
	fseek(_hFile, REWIND_SIZE, SEEK_CUR); // PreLoad에서 읽었던 값을 다시 되감는다.
	ReadSize = fread_s(
		_FReadData.GetData() + Offset,
		_FReadData.GetSize(),
		1,
		NewSize,
		_hFile);
	if (ReadSize != NewSize)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}


	_AssetNames.Reserve(_FileHeader.SerializableAssetCnt);
	Offset += FillHashersFromData(_AssetNames, _FReadData, Offset);
	if (_AssetNames.GetSize() != _FileHeader.SerializableAssetCnt)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}

	SS::StringW NSBoundAssetName;
	for (int32 i=0;i<_AssetNames.GetSize();i++)
	{
		NSBoundAssetName = _TargetDBNameSpace.C_Str();
		NSBoundAssetName += L"/";
		NSBoundAssetName += _AssetNames[i].C_Str();
		_AssetNames[i] = NSBoundAssetName.C_Str();
	}


	_AssetOffsets.SetSizeDirectly(_FileHeader.SerializableAssetCnt);
	ReadBytes = FillMemoryFromData(_AssetOffsets.GetData(), OffsetTableBytes, _FReadData, Offset);
	Offset += ReadBytes;
	if (ReadBytes != OffsetTableBytes)
	{
		SS_ASSERT(false);
		HandleFailOnLoad();
		return;
	}


	_FReadData.Clear();
}

void ApakFileReader::Release()
{
	if (_hFile != nullptr)
	{
		fclose(_hFile);
		_hFile = nullptr;
	}

	delete this;
}

SS::SHasherW ApakFileReader::GetApakAssetName(int32 Idx) const
{
	return _AssetNames[Idx];
}

SS::SHasherW ApakFileReader::GetFilePath()
{
	return _FilePath;
}

int32 ApakFileReader::GetApakAssetCnt() const
{
	return _FileHeader.SerializableAssetCnt;
}

bool ApakFileReader::IsValid() const
{
	return _hFile != nullptr;
}

const SS::PooledList<byte>& ApakFileReader::GetCursoredData() const
{
	return _FReadData;
}

bool ApakFileReader::SetDataCursorToAsset(SS::SHasherW AssetName)
{
	_FReadData.Clear();

	int32 i = 0;
	int32 AssetCnt = _AssetNames.GetSize();
	for (; i < AssetCnt; i++)
	{
		if (_AssetNames[i] == AssetName)
		{
			break;
		}
	}

	if (i >= AssetCnt)
	{
		SS_ASSERT(false);
		return false; 
	}


	ApakDataChunkOffsetDesc ChunkDesc = _AssetOffsets[i];
	fseek(_hFile, ChunkDesc.Offset, SEEK_SET);
	_FReadData.SetSizeDirectly(ChunkDesc.Size);

	int64 ReadSize = fread_s(
		_FReadData.GetData(),
		_FReadData.GetSize(),
		1,
		ChunkDesc.Size,
		_hFile);
	if (ReadSize != ChunkDesc.Size)
	{
		SS_ASSERT(false);
		return false;
	}

	return true;
}

void ApakFileReader::HandleFailOnLoad()
{
	if (_hFile != nullptr)
	{
		fclose(_hFile);
		_hFile = nullptr;
	}

	_FilePath = SS::SHasherW();
	_FileHeader.SerializableAssetCnt = 0;
	_FileHeader.FileTotalBytes = 0;
	_FReadData.Clear();
}
