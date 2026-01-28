#define SSRENDERER_MODULE_EXPORT
#include "RenderAssetSerializeFunctions.h"

#include "MeshAssetSerializer.h"
#include "SSRenderer/Private/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

int FillMeshDefaultData(SS::PooledList<byte>& Data, const MeshRawDataDefault* MeshDefaultData)
{
	if (MeshDefaultData->_vertexData == nullptr ||
	MeshDefaultData->_indexData == nullptr)
	{
		return -1;
	}

	constexpr int32 VertexHeaderSize = sizeof(MeshRawDataVertexHeader);

	const int32 EachVertexSize = EachVertexSizeOfType(MeshDefaultData->GetMeshType());
	const int32 VertexDataSize = EachVertexSize * MeshDefaultData->_VertexHeader.vertexCnt;

	const int32 IndexDataSize = MeshDefaultData->_VertexHeader.wholeIndexDataCnt * sizeof(uint32);

	const int32 StreamSize = sizeof(int32) + VertexHeaderSize + VertexDataSize + IndexDataSize;
	// 전체 데이터 크기 = 데이터크기4byte + 헤더크기 + 버텍스버퍼크기 + 인덱스버퍼크기


	int32 Cursor = 0;

	Data.SetSizeDirectly(StreamSize);
	byte* RawData = Data.GetData();

	errno_t Result = 0;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		&StreamSize, sizeof(int32));
	Cursor += sizeof(int32);

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		&MeshDefaultData->_VertexHeader, VertexHeaderSize);
	Cursor += VertexHeaderSize;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		MeshDefaultData->_vertexData, VertexDataSize);
	Cursor += VertexDataSize;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		MeshDefaultData->_indexData, IndexDataSize);
	Cursor += IndexDataSize;


	if (Result != 0)
	{
		return -1;
	}

	return Cursor;
}

int FillMeshSkinnedData(SS::PooledList<byte>& Data, int Offset, const MeshRawDataSkinned* MeshSkinnedData)
{
	if (MeshSkinnedData->_vertexData == nullptr ||
		MeshSkinnedData->_indexData == nullptr ||
		MeshSkinnedData->_BonePlacements.GetSize() == 0)
	{
		return -1;
	}

	constexpr int32 VertexHeaderSize = sizeof(MeshRawDataVertexHeader);
	constexpr int32 BoneHeaderSize = sizeof(MeshRawDataBoneHeader);

	const int32 EachVertexSize = EachVertexSizeOfType(MeshSkinnedData->GetMeshType());
	const int32 VertexDataSize = EachVertexSize * MeshSkinnedData->_VertexHeader.vertexCnt;

	const int32 IndexDataSize = MeshSkinnedData->_VertexHeader.wholeIndexDataCnt * sizeof(uint32);

	const int32 BoneDataSize = MeshSkinnedData->_BoneHeader.

	const int32 StreamSize = sizeof(int32) + VertexHeaderSize + BoneHeaderSize + VertexDataSize + IndexDataSize;
	// 전체 데이터 크기 = 데이터크기4byte + 버텍스인덱스헤더크기 + 본헤더크기 + 버텍스버퍼크기 + 인덱스버퍼크기


	int32 Cursor = 0;

	Data.SetSizeDirectly(StreamSize);
	byte* RawData = Data.GetData();

	errno_t Result = 0;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		&StreamSize, sizeof(int32));
	Cursor += sizeof(int32);

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		&MeshSkinnedData->_VertexHeader, VertexHeaderSize);
	Cursor += VertexHeaderSize;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		MeshSkinnedData->_vertexData, VertexDataSize);
	Cursor += VertexDataSize;

	Result |= memcpy_s(RawData + Cursor, StreamSize - Cursor,
		MeshSkinnedData->_indexData, IndexDataSize);
	Cursor += IndexDataSize;


	if (Result != 0)
	{
		return -1;
	}

	return Cursor;
}


bool FillDataFromAsset(MeshDataSerializerContainer& Container)
{
	Container.Data.Clear();

	const MeshRawDataBase* MeshRawData = Container.MeshData;

	EMeshType Type = MeshRawData->GetMeshType();

	int WrittenBytes = 0;
	if (Type == EMeshType::Rigid)
	{
		const MeshRawDataDefault* MeshDefaultData = static_cast<const MeshRawDataDefault*>(MeshRawData);
		WrittenBytes = FillMeshDefaultData(Container.Data, MeshDefaultData);
	}
	else if (Type == EMeshType::Skinned)
	{
		const MeshRawDataSkinned* MeshSkinnedData = static_cast<const MeshRawDataSkinned*>(MeshRawData);
		WrittenBytes = FillMeshSkinnedData(Container.Data, MeshSkinnedData);
	}

	SS_ASSERT(WrittenBytes == Container.Data.GetSize());
	return WrittenBytes == Container.Data.GetSize();
}

bool FillAssetFromData(MeshDataSerializerContainer& Container)
{
}
