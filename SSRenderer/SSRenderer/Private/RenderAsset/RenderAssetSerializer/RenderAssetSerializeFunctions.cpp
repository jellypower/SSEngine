#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"

#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"

#include "SSRenderer/Private/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"

int AppendDataFromDefaultMesh(SS::PooledList<byte>& Data, const MeshRawDataDefault* MeshDefaultData)
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
	Data.Reserve(OriginalSize + StreamSizeToFill);

	int32 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &StreamSizeToFill, sizeof(int32));
	WrittenBytes += AppendData(Data, &MeshDefaultData->_VertexHeader, VertexHeaderSize);
	WrittenBytes += AppendData(Data, MeshDefaultData->_vertexData, VertexDataSize);
	WrittenBytes += AppendData(Data, MeshDefaultData->_indexData, IndexDataSize);


	if (StreamSizeToFill != WrittenBytes)
	{
		SS_ASSERT(false);
		return 0;
	}

	return WrittenBytes;
}

int AppendDataFromSkinnedMeshBone(SS::PooledList<byte>& Data, const MeshRawDataSkinned* MeshSkinnedData)
{
	const int32 BonePlacementsCnt = MeshSkinnedData->_BonePlacements.GetSize();
	const int32 BoneNameCnt = MeshSkinnedData->_BoneNames.GetSize();
	if (BonePlacementsCnt == 0 ||
		BoneNameCnt == 0)
	{
		SS_ASSERT(false);
		return 0;
	}

	SS_ASSERT(BonePlacementsCnt == BoneNameCnt);

	constexpr int32 BoneHeaderSize = sizeof(MeshRawDataBoneHeader);

	const int32 BoneCnt = MeshSkinnedData->_BoneHeader._BoneCnt;
	SS_ASSERT(BoneCnt == BonePlacementsCnt);
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
	Data.Reserve(OriginalSize + StreamSize);

	int32 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &StreamSize, sizeof(int32));
	WrittenBytes += AppendData(Data, &MeshSkinnedData->_BoneHeader, BoneHeaderSize);
	WrittenBytes += AppendDataFromHashers(Data, MeshSkinnedData->_BoneNames);
	WrittenBytes += AppendData(Data, MeshSkinnedData->_BonePlacements.GetData(), BoneTransformStreamSize);

	if (WrittenBytes != StreamSize)
	{
		SS_ASSERT(false);
		return 0;
	}

	return WrittenBytes;
}


int AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData)
{
	const int OriginalDataSize = Data.GetSize();

	const MeshRawDataBase* MeshRawData = MeshData;

	const EMeshType Type = MeshRawData->GetMeshType();



	int32 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &WrittenBytes, sizeof(int32));
	// 첫 4바이트에 전체 데이터 사이즈를 우선 0으로 적어줍니다.

	if (Type == EMeshType::Rigid || Type == EMeshType::Skinned)
	{
		const MeshRawDataDefault* MeshDefaultData = static_cast<const MeshRawDataDefault*>(MeshRawData);
		WrittenBytes += AppendDataFromDefaultMesh(Data, MeshDefaultData);
	}

	if (Type == EMeshType::Skinned)
	{
		const MeshRawDataSkinned* MeshSkinnedData = static_cast<const MeshRawDataSkinned*>(MeshRawData);
		WrittenBytes += AppendDataFromSkinnedMeshBone(Data, MeshSkinnedData);
	}

	const int CurDataSize = Data.GetSize();
	SS_ASSERT(WrittenBytes == CurDataSize - OriginalDataSize);

	memcpy_s(Data.GetData() + OriginalDataSize, sizeof(int32), &WrittenBytes, sizeof(int32));
	// 실제로 적어낸 데이터 사이즈를 다시 기입해줍니다.

	return WrittenBytes;
}


int FillMeshVertexDataOnly(MeshRawDataDefault* MeshDataToFill, const MeshRawDataVertexHeader& DecodedHeader, const SS::PooledList<byte>& Data, const int Offset)
{
	const int32 EachVertexSize = EachVertexSizeOfType(DecodedHeader.MeshType);
	const int32 VertexDataSize = EachVertexSize * DecodedHeader.vertexCnt;

	const int32 IndexDataSize = DecodedHeader.wholeIndexDataCnt * sizeof(uint32);

	const int32 StreamSizeToFill = VertexDataSize + IndexDataSize;


	if (MeshDataToFill->_vertexData != nullptr)
	{
		free(MeshDataToFill->_vertexData);
	}

	if (MeshDataToFill->_indexData != nullptr)
	{
		free(MeshDataToFill->_indexData);
	}

	MeshDataToFill->_vertexData = DBG_MALLOC(VertexDataSize);
	MeshDataToFill->_indexData = (uint32*)DBG_MALLOC(IndexDataSize);

	int32 WrittenBytes = 0;
	WrittenBytes += FillMemoryFromData(MeshDataToFill->_vertexData, VertexDataSize, Data, Offset + WrittenBytes);
	WrittenBytes += FillMemoryFromData(MeshDataToFill->_indexData, IndexDataSize, Data, Offset + WrittenBytes);

	return WrittenBytes;
}

int FillMeshBoneDataWithHeader(MeshRawDataSkinned* MeshDataToFill, const SS::PooledList<byte>& Data, const int Offset)
{
	int32 BoneDataSizeIncludingHeader = 0;

	int32 WrittenBytes = 0;
	WrittenBytes += FillMemoryFromData(&BoneDataSizeIncludingHeader, sizeof(int32), Data, Offset + WrittenBytes);
	WrittenBytes += FillMemoryFromData(&MeshDataToFill->_BoneHeader, sizeof(MeshRawDataBoneHeader), Data, Offset + WrittenBytes);


	const int32 ReadBoneCnt = MeshDataToFill->_BoneHeader._BoneCnt;

	WrittenBytes += FillHashersFromData(MeshDataToFill->_BoneNames, Data, Offset + WrittenBytes);

	MeshDataToFill->_BonePlacements.SetSizeDirectly(ReadBoneCnt);
	WrittenBytes += FillMemoryFromData(
		MeshDataToFill->_BonePlacements.GetData(),
		sizeof(Transform) * ReadBoneCnt,
		Data,
		Offset + WrittenBytes);
	
	SS_ASSERT(ReadBoneCnt == MeshDataToFill->_BoneNames.GetSize());
	return WrittenBytes;
}

int FillMeshAssetFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int Offset)
{
	int32 WholeMeshDataSize = 0;
	Offset += FillMemoryFromData(&WholeMeshDataSize, sizeof(int32), Data, Offset);

	int32 VertexDataSize = 0; // VertexBuffer + IndexBuffer + Header size
	MeshRawDataVertexHeader DecodedHeader;
	Offset += FillMemoryFromData(&VertexDataSize, sizeof(int32), Data, Offset);
	Offset += FillMemoryFromData(&DecodedHeader, sizeof(MeshRawDataVertexHeader), Data, Offset);



	MeshRawDataDefault* RawDataToFill = nullptr;
	if (InOutMeshRawData == nullptr)// Create If null
	{
		if (DecodedHeader.MeshType == EMeshType::Rigid)
		{
			RawDataToFill = DBG_NEW MeshRawDataDefault;
		}
		else if (DecodedHeader.MeshType == EMeshType::Skinned)
		{
			RawDataToFill = DBG_NEW MeshRawDataSkinned;
		}

		InOutMeshRawData->_VertexHeader = DecodedHeader;
		InOutMeshRawData = RawDataToFill;
	}
	else
	{
		if (DecodedHeader.MeshType != InOutMeshRawData->GetMeshType())
		{
			SS_ASSERT_MSG(false, L"Class Type Of MeshDataToFill does not Match");
			return 0;
		}

		InOutMeshRawData->_VertexHeader = DecodedHeader;
		RawDataToFill = InOutMeshRawData;
	}

	Offset += FillMeshVertexDataOnly(RawDataToFill, DecodedHeader, Data, Offset);

	if (DecodedHeader.MeshType == EMeshType::Skinned)
	{
		MeshRawDataSkinned* SkinnedRawData = static_cast<MeshRawDataSkinned*>(RawDataToFill);
		Offset += FillMeshBoneDataWithHeader(SkinnedRawData, Data, Offset);
	}
	
	return Offset;
}
