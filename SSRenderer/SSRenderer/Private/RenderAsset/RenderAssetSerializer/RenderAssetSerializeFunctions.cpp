#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetStrUtil.h"


#include "ApakFileReader.h"
#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"

#include "SSRenderer/Public/RenderAssetSerializer/ApakDataChunkOffsetDesc.h"
#include "SSRenderer/Private/RenderAsset/RenderAssetType/MeshAsset.h"
#include "SSRenderer/Private/RenderAsset/RenderAssetType/ModelCombinationAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"


int64 AppendDataFromDefaultMesh(SS::PooledList<byte>& Data, const MeshRawDataDefault* MeshDefaultData)
{
	if (MeshDefaultData->_vertexData == nullptr ||
		MeshDefaultData->_indexData == nullptr)
	{
		SS_ASSERT(false);
		return 0;
	}

	constexpr int64 VertexHeaderSize = sizeof(MeshRawDataVertexHeader);

	const int64 EachVertexSize = EachVertexSizeOfType(MeshDefaultData->GetMeshType());
	const int64 VertexDataSize = EachVertexSize * MeshDefaultData->_VertexHeader.vertexCnt;

	const int64 IndexDataSize = MeshDefaultData->_VertexHeader.wholeIndexDataCnt * sizeof(uint32);

	const int64 StreamSizeToFill = sizeof(int64) + VertexHeaderSize + VertexDataSize + IndexDataSize;
	// 전체 데이터 크기 = 데이터크기4byte + 헤더크기 + 버텍스버퍼크기 + 인덱스버퍼크기


	int64 OriginalSize = Data.GetSize();
	Data.Reserve(OriginalSize + StreamSizeToFill);

	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &StreamSizeToFill, sizeof(StreamSizeToFill));
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

int64 AppendDataFromSkinnedMeshBone(SS::PooledList<byte>& Data, const MeshRawDataSkinned* MeshSkinnedData)
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
	BoneStrStreamSize += sizeof(int64);
	for (int32 i = 0; i < BoneCnt; i++)
	{
		int BoneNameLen = MeshSkinnedData->_BoneNames[i].GetStrLen();
		BoneStrStreamSize += (BoneNameLen + 1) * sizeof(utf16);
	}


	const int32 BoneTransformStreamSize = BoneCnt * sizeof(Transform);
	const int64 StreamSize = sizeof(int64) + BoneHeaderSize + BoneStrStreamSize + BoneTransformStreamSize;
	// 전체 데이터 크기 = 스트림사이즈8byte + 본헤더크기 + 본 데이터 크기


	int64 OriginalSize = Data.GetSize();
	Data.Reserve(OriginalSize + StreamSize);

	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &StreamSize, sizeof(StreamSize));
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


int64 AppendDataFromMeshAsset(
	SS::PooledList<byte>& Data,
	const MeshRawDataBase* MeshData)
{
	const int64 OriginalDataSize = Data.GetSize();

	const MeshRawDataBase* MeshRawData = MeshData;

	const EMeshType Type = MeshRawData->GetMeshType();



	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &WrittenBytes, sizeof(WrittenBytes));
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

	const int64 CurDataSize = Data.GetSize();
	SS_ASSERT(WrittenBytes == CurDataSize - OriginalDataSize);

	memcpy_s(Data.GetData() + OriginalDataSize, sizeof(WrittenBytes), &WrittenBytes, sizeof(WrittenBytes));
	// 실제로 적어낸 데이터 사이즈를 다시 기입해줍니다.

	return WrittenBytes;
}

int64 AppendDataFromMdlcAsset(
	SS::PooledList<byte>& Data,
	const IModelCombinationAsset* MdlcData,
	SS::SHasherW MdlcChildNameSpaceReplaced,
	SS::SHasherW MdlcChildNameSpaceToReplace)
{
	const int64 OriginalDataSize = Data.GetSize();

	const int32 ChildCnt = MdlcData->GetChildCnt();
	SS::PooledList<SS::SHasherW> PlacementNames(ChildCnt);
	SS::PooledList<SS::SHasherW> AssetNames(ChildCnt);
	SS::PooledList<Transform> Transforms(ChildCnt);
	SS::PooledList<EMeshType> MeshTypes(ChildCnt);
	SS::PooledList<int32> ParentIndices(ChildCnt);

	for (int32 i = 0; i < ChildCnt; i++)
	{
		const AssetPlacementReference& Item = MdlcData->GetChildAt(i);
		if (MdlcChildNameSpaceReplaced.IsEmpty() || Item.AssetName.IsEmpty())
		{
			AssetNames.PushBack(Item.AssetName);
		}
		else
		{
			SS::SHasherW ConvertedAssetName =
				ReplaceAssestNameNameSpace(Item.AssetName, MdlcChildNameSpaceReplaced, MdlcChildNameSpaceToReplace);;
			AssetNames.PushBack(ConvertedAssetName);
		}

		PlacementNames.PushBack(Item.PlacementName);
		Transforms.PushBack(Item.Transform);
		MeshTypes.PushBack(Item.MeshType);
		ParentIndices.PushBack(Item.ParentIdx);
	}

	MdlcAssetHeader Header = MdlcData->GetAssetHeader();

	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &WrittenBytes, sizeof(WrittenBytes));
	WrittenBytes += AppendData(Data, &Header, sizeof(Header));
	WrittenBytes += AppendDataFromHashers(Data, PlacementNames);
	WrittenBytes += AppendDataFromHashers(Data, AssetNames);
	WrittenBytes += AppendData(Data, Transforms.GetData(), sizeof(Transform) * ChildCnt);
	WrittenBytes += AppendData(Data, MeshTypes.GetData(), sizeof(EMeshType) * ChildCnt);
	WrittenBytes += AppendData(Data, ParentIndices.GetData(), sizeof(int32) * ChildCnt);

	memcpy_s(Data.GetData() + OriginalDataSize, sizeof(WrittenBytes),
		&WrittenBytes, sizeof(WrittenBytes));

	return WrittenBytes;
}

int64 AppendDataFromRenderAnim(SS::PooledList<byte>& Data, const RenderAnimRawData* AnimData)
{
	const int32 TrackCnt = AnimData->_Header.TrackCnt;
	SS_ASSERT(TrackCnt == AnimData->_Tracks.GetSize());

	const int64 OriginalDataSize = Data.GetSize();


	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &WrittenBytes, sizeof(WrittenBytes));
	// 1. 전체 데이터 사이즈를 우선 0으로 적어줍니다.

	WrittenBytes += AppendData(Data, &AnimData->_Header, sizeof(AnimDataHeader));
	// 2. 헤더 적어주기


	SS::PooledList<SS::SHasherW> TrackNames(TrackCnt);

	for (int32 i = 0; i < TrackCnt; i++)
	{
		SS::SHasherW TrackNameItem = AnimData->_Tracks[i]._TrackName;
		SS_ASSERT(TrackNameItem.IsEmpty() == false);
		TrackNames.PushBack(TrackNameItem);
	}

	WrittenBytes += AppendDataFromHashers(Data, TrackNames);
	// 3. 트랙 이름들

	for (int32 iTrack = 0; iTrack < TrackCnt; iTrack++)
	{
		const RKFTrack& Track = AnimData->_Tracks[iTrack];
		const int32 TrackItemCnt = Track._TrackItems.GetSize();
		RKFTrackItem* TrackItemsRaw = Track._TrackItems.GetData();


		WrittenBytes += AppendData(Data, &TrackItemCnt, sizeof(TrackItemCnt));
		// 4. 각 트랙 카운트


		const int32 DataSize = sizeof(RKFTrackItem) * TrackItemCnt;
		WrittenBytes += AppendData(Data, &TrackItemsRaw, DataSize);
		// 5. 각 트랙 데이터
	}


	const int64 CurDataSize = Data.GetSize();
	SS_ASSERT(WrittenBytes == CurDataSize - OriginalDataSize);
	memcpy_s(Data.GetData() + OriginalDataSize, sizeof(WrittenBytes), &WrittenBytes, sizeof(WrittenBytes));
	// 6. 실제로 적어낸 데이터 사이즈를 다시 기입해줍니다.

	return WrittenBytes;
}


int64 FillMeshVertexDataOnly(MeshRawDataDefault* MeshDataToFill, const MeshRawDataVertexHeader& DecodedHeader, const SS::PooledList<byte>& Data, const int64 Offset)
{
	const int32 EachVertexSize = EachVertexSizeOfType(DecodedHeader.MeshType);
	const int32 VertexDataSize = EachVertexSize * DecodedHeader.vertexCnt;

	const int32 IndexDataSize = DecodedHeader.wholeIndexDataCnt * sizeof(uint32);


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

	int64 WrittenBytes = 0;
	WrittenBytes += FillMemoryFromData(MeshDataToFill->_vertexData, VertexDataSize, Data, Offset + WrittenBytes);
	WrittenBytes += FillMemoryFromData(MeshDataToFill->_indexData, IndexDataSize, Data, Offset + WrittenBytes);

	return WrittenBytes;
}

int64 FillMeshBoneDataWithHeader(MeshRawDataSkinned* MeshDataToFill, const SS::PooledList<byte>& Data, const int64 Offset)
{
	int64 BoneDataSizeIncludingHeader = 0;

	int64 WrittenBytes = 0;
	WrittenBytes += FillMemoryFromData(&BoneDataSizeIncludingHeader, sizeof(BoneDataSizeIncludingHeader), Data, Offset + WrittenBytes);
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


int64 FillMeshAssetHeaaderOnly(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int64 Offset)
{
	int64 OriginalOffset = Offset;
	int64 WholeMeshDataSize = 0; // Vertex/Index + BoneStructure + (BLAS cache, etc) size
	Offset += FillMemoryFromData(&WholeMeshDataSize, sizeof(WholeMeshDataSize), Data, Offset);

	int64 VertexDataSize = 0; // 자기자신 + Header +  VertexBuffer + IndexBuffer
	MeshRawDataVertexHeader DecodedHeader;
	Offset += FillMemoryFromData(&VertexDataSize, sizeof(VertexDataSize), Data, Offset);
	Offset += FillMemoryFromData(&DecodedHeader, sizeof(DecodedHeader), Data, Offset);

	SS_ASSERT(
		VertexDataSize ==
		sizeof(int64) +
		EachVertexSizeOfType(DecodedHeader.MeshType) * DecodedHeader.vertexCnt +
		sizeof(uint32) * DecodedHeader.wholeIndexDataCnt +
		sizeof(MeshRawDataVertexHeader));

	if (InOutMeshRawData == nullptr)// Create If null
	{
		if (DecodedHeader.MeshType == EMeshType::Rigid)
		{
			InOutMeshRawData = DBG_NEW MeshRawDataDefault;
		}
		else if (DecodedHeader.MeshType == EMeshType::Skinned)
		{
			InOutMeshRawData = DBG_NEW MeshRawDataSkinned;
		}
		else
		{
			SS_INTERRUPT();
		}

		InOutMeshRawData->_VertexHeader = DecodedHeader;
	}
	else
	{
		if (DecodedHeader.MeshType != InOutMeshRawData->GetMeshType())
		{
			SS_ASSERT_MSG(false, L"Class Type Of MeshDataToFill does not Match");
			return 0;
		}

		InOutMeshRawData->_VertexHeader = DecodedHeader;
	}

	return Offset - OriginalOffset;
}

int64 FillMeshRawDataFromData(
	MeshRawDataDefault*& InOutMeshRawData,
	const SS::PooledList<byte>& Data,
	int64 Offset)
{
	int64 OriginalOffset = Offset;
	Offset += FillMeshAssetHeaaderOnly(InOutMeshRawData, Data, Offset); // Create InOutMeshRawData if null only with header
	Offset += FillMeshVertexDataOnly(InOutMeshRawData, InOutMeshRawData->_VertexHeader, Data, Offset);

	if (InOutMeshRawData->_VertexHeader.MeshType == EMeshType::Skinned)
	{
		MeshRawDataSkinned* SkinnedRawData = static_cast<MeshRawDataSkinned*>(InOutMeshRawData);
		Offset += FillMeshBoneDataWithHeader(SkinnedRawData, Data, Offset);
	}

	int64 WrittenBytes = Offset - OriginalOffset;
	return WrittenBytes;
}

int64 FillRenderAnimFromData(RenderAnimRawData*& OutAnimData, const SS::PooledList<byte>& Data, int64 Offset)
{
	if (OutAnimData != nullptr)
	{
		SS_INTERRUPT();
	}
	RenderAnimRawData* AnimDataToFill = DBG_NEW RenderAnimRawData;
	OutAnimData = AnimDataToFill;


	const int64 OriginalOffset = Offset;


	int64 WholeAnimDataSize = 0;
	Offset += FillMemoryFromData(&WholeAnimDataSize, sizeof(WholeAnimDataSize), Data, Offset);
	// 1. 전체 데이터 사이즈
	Offset += FillMemoryFromData(&AnimDataToFill->_Header, sizeof(AnimDataToFill->_Header), Data, Offset);
	// 2. 트랙 헤더

	SS::PooledList<SS::SHasherW> TrackNames;
	Offset += FillHashersFromData(TrackNames, Data, Offset);
	// 3. 트랙 이름들 읽어오기

	if (AnimDataToFill->_Header.TrackCnt != TrackNames.GetSize()) SS_INTERRUPT();
	const int32 TrackCnt = AnimDataToFill->_Header.TrackCnt;

	AnimDataToFill->_Tracks.Resize(TrackCnt);
	for (int32 i=0;i<TrackCnt;i++)
	{
		AnimDataToFill->_Tracks[i]._TrackName = TrackNames[i];
		// 3. 트랙 이름들 채우기
	}


	for (int32 i = 0; i < TrackCnt; i++)
	{
		int32 TrackItemCnt;
		Offset += FillMemoryFromData(&TrackItemCnt, sizeof(TrackItemCnt), Data, Offset);
		// 4. 각 트랙 카운트 읽어오기

		RKFTrack& ThisTrack = AnimDataToFill->_Tracks[i];
		ThisTrack._TrackItems.SetSizeDirectly(TrackItemCnt);
		RKFTrackItem* TrackItemsRaw = ThisTrack._TrackItems.GetData();

		const int32 DataSize = TrackItemCnt * sizeof(RKFTrackItem);
		Offset += FillMemoryFromData(TrackItemsRaw, DataSize, Data, Offset);
		// 5. 각 트랙 데이터 채우기
	}

	if (Offset - OriginalOffset != WholeAnimDataSize) SS_INTERRUPT();
	return Offset - OriginalOffset;
}

int64 AppendApakDataFromAssetList(
	SS::PooledList<byte>& Data,
	const SS::PooledList<IAssetBase*>& AssetListToSerailize,
	SS::SHasherW MdlcChildNameSpaceReplaced,
	SS::SHasherW MdlcChildNameSpaceToReplace)
{
	int64 SerializeTrialCnt = AssetListToSerailize.GetSize();

	SS::PooledList<IAssetBase*> SerializableAssets(SerializeTrialCnt);
	SS::PooledList<SS::SHasherW> SerializedAssetNames(SerializeTrialCnt);

	// Find Serializable Assets
	for (IAssetBase* AssetItem : AssetListToSerailize)
	{
		EAssetType AssetItemType = AssetItem->GetAssetType();
		if (AssetItemType != EAssetType::Mesh &&
			AssetItemType != EAssetType::ModelCombination)
		{
			SS_ASSERT(false);
			continue;
		}

		SerializableAssets.PushBack(AssetItem);

		// Apak파일 안에있는 각 에셋들은 나중에 로드할 에셋의 네임스페이스를 따라서 이름이 붙는다.
		// 그래서 네임스페이스를 떼고 명단에 넣어준다.
		SS::SHasherW ConvertedAssetName =
			ReplaceAssestNameNameSpace(AssetItem->GetAssetName(), MdlcChildNameSpaceReplaced, SS::SHasherW());

		SerializedAssetNames.PushBack(ConvertedAssetName);
	}

	// Start Serialize
	const int64 OriginalDataSize = Data.GetSize();

	ApakFileHeader Header;
	Header.FileTotalBytes = OriginalDataSize;
	Header.SerializableAssetCnt = SerializableAssets.GetSize();
	Header.Padd1 = -1;
	Header.Padd2 = -1;


	int64 Offset = OriginalDataSize;
	Offset += AppendData(Data, &Header, sizeof(ApakFileHeader));


	// Serialize AssetNameTable
	Offset += AppendDataFromHashers(Data, SerializedAssetNames);

	// Alloc Serialize Data Chunk offset

	const int32 DataChunkOffsetDescOffset = Data.GetSize();
	int32 NewDataSize = sizeof(ApakDataChunkOffsetDesc) * Header.SerializableAssetCnt;
	Data.SetSizeDirectly(DataChunkOffsetDescOffset + NewDataSize);
	Offset += NewDataSize;


	// Serialize Assets
	for (int64 i = 0; i < Header.SerializableAssetCnt; i++)
	{
		IAssetBase* AssetItem = SerializableAssets[i];
		EAssetType AssetItemType = AssetItem->GetAssetType();

		int32 PrevOffset = Data.GetSize();
		int32 WrittenByteItem = 0;

		if (AssetItemType == EAssetType::Mesh)
		{
			IMeshAsset* MeshAssetItem = static_cast<IMeshAsset*>(AssetItem);
			const MeshRawDataBase* MeshRawData = MeshAssetItem->GetMeshRawData();
			WrittenByteItem = AppendDataFromMeshAsset(Data, MeshRawData);
		}
		else if (AssetItemType == EAssetType::ModelCombination)
		{
			IModelCombinationAsset* MdlcAssetItem = static_cast<IModelCombinationAsset*>(AssetItem);
			WrittenByteItem = AppendDataFromMdlcAsset(
				Data,
				MdlcAssetItem,
				MdlcChildNameSpaceReplaced,
				MdlcChildNameSpaceToReplace);
		}
		else
		{
			SS_ASSERT(false);
		}

		ApakDataChunkOffsetDesc* DataChunkOffsetDescRaw = reinterpret_cast<ApakDataChunkOffsetDesc*>(Data.GetData() + DataChunkOffsetDescOffset);
		DataChunkOffsetDescRaw = DataChunkOffsetDescRaw + i;
		DataChunkOffsetDescRaw->Offset = PrevOffset;
		DataChunkOffsetDescRaw->Size = WrittenByteItem;

		Offset += WrittenByteItem;
	}

	int32 TotalWrittenSize = Offset - OriginalDataSize;

	// DEBUG
	void* RawData = Data.GetData();
	ApakDataChunkOffsetDesc* DESC_FOR_DEBUG = reinterpret_cast<ApakDataChunkOffsetDesc*>(Data.GetData() + DataChunkOffsetDescOffset);
	// ~DEBUG

	Header.FileTotalBytes = TotalWrittenSize;
	memcpy_s(Data.GetData() + OriginalDataSize, sizeof(ApakFileHeader), &Header, sizeof(ApakFileHeader));
	return TotalWrittenSize;
}

int64 CreateAssetsFromApakData(
	SS::PooledList<IAssetBase*>& CreatedAssetList,
	const SS::PooledList<byte>& Data,
	SS::SHasherW ApakAssetPath,
	SS::SHasherW AssetNamespace,
	int Offset)
{
	const int32 OriginalOffset = Offset;

	ApakFileHeader Header;
	Offset += FillMemoryFromData(&Header, sizeof(ApakFileHeader), Data, Offset);


	SS::PooledList<SS::SHasherW> SerializedAssetNames(Header.SerializableAssetCnt);
	Offset += FillHashersFromData(SerializedAssetNames, Data, Offset);



	SS::PooledList<ApakDataChunkOffsetDesc> AssetOffsets;
	AssetOffsets.SetSizeDirectly(Header.SerializableAssetCnt);
	const int64 AssetOffsetDataSize = Header.SerializableAssetCnt * sizeof(ApakDataChunkOffsetDesc);
	Offset += FillMemoryFromData(AssetOffsets.GetData(), AssetOffsetDataSize, Data, Offset);


	for (int32 i = 0; i < Header.SerializableAssetCnt; i++)
	{
		ApakDataChunkOffsetDesc OffsetDescItem = AssetOffsets[i];
		SS::SHasherW AssetNameItem = SerializedAssetNames[i];
		const EAssetType AssetTypeItem = ExtractAssetTypeFromName(AssetNameItem);

		IAssetBase* NewAsset = nullptr;
		int32 ReadBytes = 0;

		const int64 ThisAssetOffset = OriginalOffset + OffsetDescItem.Offset;

		if (AssetTypeItem == EAssetType::Mesh)
		{
			IMeshAsset* NewMeshAsset = nullptr;
			ReadBytes = CreateMeshAssetFromData(
				NewMeshAsset,
				AssetNameItem,
				ApakAssetPath,
				AssetNamespace,
				Data,
				ThisAssetOffset);
			NewAsset = NewMeshAsset;
		}
		else
		{
			SS_ASSERT_MSG(false, "TODO: Impl");
		}

		if (ReadBytes != OffsetDescItem.Size)
		{
			SS_ASSERT(false);
			continue;
		}

		Offset += ReadBytes;
		CreatedAssetList.PushBack(NewAsset);
	}

	return Offset;
}

int64 CreateMeshAssetFromData(
	IMeshAsset*& OutMeshAsset,
	SS::SHasherW AssetName,
	SS::SHasherW AssetPath,
	SS::SHasherW AssetNamespace,
	const SS::PooledList<byte>& Data,
	int Offset)
{
	// 껍데기만 만들고
	MeshAsset* NewAsset = DBG_NEW MeshAsset(AssetNamespace, AssetName, AssetPath);


	// 실제 Raw데이터 만들어서 삽입
	MeshRawDataDefault* CreatedDefaultData = nullptr;
	int64 AssetSize = FillMeshRawDataFromData(CreatedDefaultData, Data, Offset);
	NewAsset->InjectRawDataXXX(CreatedDefaultData);

	OutMeshAsset = NewAsset;
	return AssetSize;
}


int64 FillEmptyMdlcAssetFromData(
	IModelCombinationAssetMutable* MdlcAssetToFill,
	const SS::PooledList<byte>& Data,
	int64 Offset)
{
	const int64 OriginalOffset = Offset;

	int64 MdlcDataWholeSize;
	Offset += FillMemoryFromData(&MdlcDataWholeSize, sizeof(MdlcDataWholeSize), Data, Offset);

	MdlcAssetHeader Header;
	Offset += FillMemoryFromData(&Header, sizeof(Header), Data, Offset);

	SS::PooledList<SS::SHasherW> PlacementNames(Header.ChildCnt);
	SS::PooledList<SS::SHasherW> PlacementAssetNames(Header.ChildCnt);
	SS::PooledList<Transform> PlacementTransforms(Header.ChildCnt);
	SS::PooledList<EMeshType> PlacementMeshTypes(Header.ChildCnt);
	SS::PooledList<int32> PlacementParentIndices(Header.ChildCnt);


	Offset += FillHashersFromData(PlacementNames, Data, Offset);
	Offset += FillHashersFromData(PlacementAssetNames, Data, Offset);
	SS_ASSERT(PlacementNames.GetSize() == PlacementAssetNames.GetSize());

	PlacementTransforms.SetSizeDirectly(Header.ChildCnt);
	PlacementMeshTypes.SetSizeDirectly(Header.ChildCnt);
	PlacementParentIndices.SetSizeDirectly(Header.ChildCnt);

	Offset += FillMemoryFromData(PlacementTransforms.GetData(), PlacementTransforms.GetSize() * sizeof(Transform), Data, Offset);
	Offset += FillMemoryFromData(PlacementMeshTypes.GetData(), PlacementMeshTypes.GetSize() * sizeof(EMeshType), Data, Offset);
	Offset += FillMemoryFromData(PlacementParentIndices.GetData(), PlacementParentIndices.GetSize() * sizeof(int32), Data, Offset);

	// 껍데기만 만들고
	MdlcAssetToFill->ClearChilds();
	MdlcAssetToFill->ReserveChilds(Header.ChildCnt);
	MdlcAssetToFill->SetHeader(Header);

	for (int32 i = 0; i < Header.ChildCnt; i++)
	{
		AssetPlacementReference NewPlacement;
		NewPlacement.PlacementName = PlacementNames[i];
		NewPlacement.AssetName = PlacementAssetNames[i];
		NewPlacement.Transform = PlacementTransforms[i];
		NewPlacement.MeshType = PlacementMeshTypes[i];
		NewPlacement.ParentIdx = PlacementParentIndices[i];
		MdlcAssetToFill->AddNewChild(NewPlacement);
	}

	SS_ASSERT(MdlcDataWholeSize == Offset - OriginalOffset);
	return MdlcDataWholeSize;
}

EAssetType ExtractAssetTypeFromName(SS::SHasherW InAssetName)
{
	const utf16* CStrAssetName = InAssetName.C_Str();
	if (CStrAssetName == nullptr)
	{
		return EAssetType::None;
	}

	const int32 StrLen = InAssetName.GetStrLen();


	const utf16* InExtentionStrStart = nullptr;
	for (int32 i = StrLen - 1; i >= 0; i--)
	{
		if (CStrAssetName[i] == L'.')
		{
			InExtentionStrStart = CStrAssetName + i;
			break;
		}
	}

	if (InExtentionStrStart == nullptr)
	{
		return EAssetType::None;
	}


	for (int32 i = (int32)EAssetType::Mesh; i < (int32)EAssetType::Count; i++)
	{
		EAssetType AssetTypeItem = static_cast<EAssetType>(i);
		const utf16* AssetSuffixItem = GetAssetSuffix(AssetTypeItem);

		if (wcscmp(InExtentionStrStart, AssetSuffixItem) == 0)
		{
			return AssetTypeItem;
		}
	}

	return EAssetType::None;
}

IApakFileReader* CreateApakFileAccessor(SS::SHasherW SystemPath, SS::SHasherW DBNameSpace)
{
	ApakFileReader* NewApak = DBG_NEW ApakFileReader(SystemPath, DBNameSpace);
	if (NewApak->IsValid() == false)
	{
		delete NewApak;
		return nullptr;
	}

	return NewApak;
}

IApakFileReader* CreateApakFileAccessorFromNameSpace(SS::SHasherW DBNameSpace, SS::SHasherW RelativePath)
{
	SS::StringW PathConstructor = L"Resource/AssetDB";
	PathConstructor += DBNameSpace.C_Str();
	PathConstructor += L"/Apak";
	PathConstructor += RelativePath.C_Str();

	SS::SHasherW FilePathName = PathConstructor.C_Str();
	return CreateApakFileAccessor(FilePathName, DBNameSpace);
}
