#define SSRENDERER_MODULE_EXPORT
#include "RenderAssetSerializeFunctions.h"

#include "SSEngineDefault/Public/CommonSerializer/StringSerializerFunctions.h"
#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializerFunctions.h"

#include "SSRenderer/Private/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

int FillDataFromDefaultMesh(SS::PooledList<byte>& Data, const MeshRawDataDefault* MeshDefaultData)
{
	if (MeshDefaultData->_vertexData == nullptr ||
		MeshDefaultData->_indexData == nullptr)
	{
		SS_ASSERT(false);
		return 0;
	}

	constexpr int32 VertexHeaderSize = sizeof(MeshRawDataVertexHeader);

	const int32 EachVertexSize = EachVertexSizeOfType(MeshDefaultData->GetMeshType());
	const int32 VertexDataSize = EachVertexSize * MeshDefaultData->_VertexHeader.vertexCnt;

	const int32 IndexDataSize = MeshDefaultData->_VertexHeader.wholeIndexDataCnt * sizeof(uint32);

	const int32 StreamSizeToFill = sizeof(int32) + VertexHeaderSize + VertexDataSize + IndexDataSize;
	// 전체 데이터 크기 = 데이터크기4byte + 헤더크기 + 버텍스버퍼크기 + 인덱스버퍼크기



	int32 OriginalSize = Data.GetSize();
	Data.SetSizeDirectly(OriginalSize + StreamSizeToFill);
	byte* RawData = Data.GetData() + OriginalSize;

	errno_t Result = 0;
	int32 Cursor = 0;

	Result |= memcpy_s(RawData + Cursor, StreamSizeToFill - Cursor,
		&StreamSizeToFill, sizeof(int32));
	Cursor += sizeof(int32);

	Result |= memcpy_s(RawData + Cursor, StreamSizeToFill - Cursor,
		&MeshDefaultData->_VertexHeader, VertexHeaderSize);
	Cursor += VertexHeaderSize;

	Result |= memcpy_s(RawData + Cursor, StreamSizeToFill - Cursor,
		MeshDefaultData->_vertexData, VertexDataSize);
	Cursor += VertexDataSize;

	Result |= memcpy_s(RawData + Cursor, StreamSizeToFill - Cursor,
		MeshDefaultData->_indexData, IndexDataSize);
	Cursor += IndexDataSize;


	if (Result != 0)
	{
		return -1;
	}

	SS_ASSERT(StreamSizeToFill == Cursor);
	return Cursor;
}

int FillBoneDataFromSkinnedMesh(SS::PooledList<byte>& Data, const MeshRawDataSkinned* MeshSkinnedData)
{
	if (MeshSkinnedData->_BonePlacements.GetSize() == 0)
	{
		SS_ASSERT(false);
		return 0;
	}

	constexpr int32 BoneHeaderSize = sizeof(MeshRawDataBoneHeader);

	const int32 BoneCnt = MeshSkinnedData->_BoneHeader._BoneCnt;
	int32 BoneStrStreamSize = 0;
	BoneStrStreamSize += sizeof(int32);
	for (int32 i = 0; i < BoneCnt; i++)
	{
		int BoneNameLen = MeshSkinnedData->_BoneNames[i].GetStrLen();
		BoneStrStreamSize += (BoneNameLen + 1) * sizeof(utf16);
	}


	const int32 BoneTransformStreamSize = BoneCnt * sizeof(Transform);
	const int32 StreamSize = sizeof(int32) + BoneHeaderSize + BoneStrStreamSize + BoneTransformStreamSize;
	// 전체 데이터 크기 = 스트림사이즈4byte + 본헤더크기 + 본 데이터 크기

	int32 OriginalSize = Data.GetSize();
	int32 ExpectedWriteResultSize = OriginalSize + StreamSize;
	Data.Reserve(ExpectedWriteResultSize);

	int32 Cursor = 0;
	errno_t Result = 0;

	Cursor += AppendData(Data, &StreamSize, sizeof(int32));
	Cursor += AppendData(Data, &MeshSkinnedData->_BoneHeader, BoneHeaderSize);
	Cursor += AppendDataFromHashers(Data, MeshSkinnedData->_BoneNames);
	Cursor += AppendData(Data, MeshSkinnedData->_BonePlacements.GetData(), BoneTransformStreamSize);

	if (Cursor != ExpectedWriteResultSize)
	{
		return 0;
	}

	return Cursor;
}


int FillDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData)
{
	int OriginalDataSize = Data.GetSize();

	const MeshRawDataBase* MeshRawData = MeshData;

	const EMeshType Type = MeshRawData->GetMeshType();

	int WrittenBytes = 0;
	if (Type == EMeshType::Rigid || Type == EMeshType::Skinned)
	{
		const MeshRawDataDefault* MeshDefaultData = static_cast<const MeshRawDataDefault*>(MeshRawData);
		WrittenBytes += FillDataFromDefaultMesh(Data, MeshDefaultData);
	}

	if (Type == EMeshType::Skinned)
	{
		const MeshRawDataSkinned* MeshSkinnedData = static_cast<const MeshRawDataSkinned*>(MeshRawData);
		WrittenBytes += FillBoneDataFromSkinnedMesh(Data, MeshSkinnedData);
	}

	int CurDataSize = Data.GetSize();
	SS_ASSERT(WrittenBytes == CurDataSize - OriginalDataSize);
	return WrittenBytes == Data.GetSize();
}

int FillMeshAssetFromData(
	MeshRawDataBase* MeshData,
	const SS::PooledList<byte>& Data,
	int Offset)
{
	return 0;
}
