#include "pch.h"
#include "MeshSerializeTest.h"

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IRenderAnimAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RenderKeyFrameAnimData/RenderAnimData.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void MeshSerializeTest(IRenderer* InRenderer, SS::SHasherW MeshToTest)
{
	IAssetManager* AssetManager = InRenderer->GetMutableAssetManager();

	IMeshAsset* MeshAsset = AssetManager->FindAssetByName<IMeshAsset>(MeshToTest);

	const MeshRawDataBase* MeshRawData = MeshAsset->GetMeshRawData();
	SS::PooledList<byte> Data;
	int64 WrittenBytes = AppendDataFromMeshAsset(Data, MeshRawData);
	SS_ASSERT(WrittenBytes > 0);


	MeshRawDataBase* MeshRawDataMutable = const_cast<MeshRawDataBase*>(MeshRawData);
	EMeshType MeshType = MeshRawDataMutable->GetMeshType();
	if (MeshType != EMeshType::Rigid && MeshType != EMeshType::Skinned)
	{
		SS_INTERRUPT();
		return;
	}

	MeshRawDataDefault* MeshRawDataDefaultMutable = static_cast<MeshRawDataDefault*>(MeshRawDataMutable);
	int64 ReadBytes = FillMeshRawDataFromData(MeshRawDataDefaultMutable, Data);

	SS_ASSERT(WrittenBytes == ReadBytes);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();
}

void MdlcSerializeTest(IRenderer* InRenderer, SS::SHasherW MdlcToTest)
{
	IAssetManagerMutable* AssetManager = InRenderer->GetMutableAssetManager();
	IModelCombinationAsset* MdlcAsset = AssetManager->FindAssetByName<IModelCombinationAsset>(MdlcToTest);

	SS::PooledList<byte> Data;
	Data.Reserve(1024);
	int64 WrittenBytes = AppendDataFromMdlcAsset(Data, MdlcAsset, SS::SHasherW(), SS::SHasherW());
	SS_ASSERT(WrittenBytes > 0);


	static const SS::SHasherW HASHSER_RUNTIME_CREATION = CRAN::NS_RUNTIME_CREATION;

	IModelCombinationAssetMutable* CreatedMdlcAsset = AssetManager->CreateEmptyModelCombinationAsset(
		HASHSER_RUNTIME_CREATION,
		MdlcAsset->GetAssetName(),
		MdlcAsset->GetAssetPath(),
		0);
	int64 ReadBytes = FillEmptyMdlcAssetFromData(CreatedMdlcAsset, Data);


	{
		const int32 ChildCnt = MdlcAsset->GetChildCnt();
		const int32 CreatedChildCnt = CreatedMdlcAsset->GetChildCnt();
		SS_ASSERT(ChildCnt == CreatedChildCnt);
		SS_ASSERT(WrittenBytes == ReadBytes);

		for (int32 i = 0; i < ChildCnt; i++)
		{
			AssetPlacementReference PrevRef = MdlcAsset->GetChildAt(i);
			AssetPlacementReference NewRef = CreatedMdlcAsset->GetChildAt(i);

			SS_ASSERT(PrevRef.PlacementName == NewRef.PlacementName);
			SS_ASSERT(PrevRef.AssetName == NewRef.AssetName);
			SS_ASSERT(PrevRef.MeshType == NewRef.MeshType);
			SS_ASSERT(PrevRef.ParentIdx == NewRef.ParentIdx);

			bool bResult = XMAlmostEqual(PrevRef.Transform.Scale.SimdVec, NewRef.Transform.Scale.SimdVec);
			SS_ASSERT(bResult);
			bResult = XMAlmostEqual(PrevRef.Transform.Rotation.SimdVec, NewRef.Transform.Rotation.SimdVec);
			SS_ASSERT(bResult);
			bResult = XMAlmostEqual(PrevRef.Transform.Position.SimdVec, NewRef.Transform.Position.SimdVec);
			SS_ASSERT(bResult);
		}
	}

	delete CreatedMdlcAsset;
}

void RenderAnimSerializeTest(IRenderer* InRenderer, SS::SHasherW RenderAnimToTest)
{
	IAssetManagerMutable* AssetManager = InRenderer->GetMutableAssetManager();
	IRenderAnimAssetMutable* AnimAsset = AssetManager->FindAssetByName<IRenderAnimAssetMutable>(RenderAnimToTest);

	SS::PooledList<byte> Data;
	Data.Reserve(1024);

	const RenderAnimRawData* OriginalRenderAnimData = AnimAsset->GetKeyFrameAnimData();
	int64 WrittenBytes = AppendDataFromRenderAnim(Data, AnimAsset->GetKeyFrameAnimData());
	SS_ASSERT(WrittenBytes > 0);

	RenderAnimRawData* NewRenderAnimData = nullptr;
	int64 ReadBytes = FillRenderAnimFromData(NewRenderAnimData, Data);
	SS_ASSERT(ReadBytes > 0);

	SS_ASSERT(WrittenBytes == ReadBytes);

	{
		SS_ASSERT(OriginalRenderAnimData->_Header.KeyFrameDuration == NewRenderAnimData->_Header.KeyFrameDuration);
		SS_ASSERT(OriginalRenderAnimData->_Header.TrackCnt == NewRenderAnimData->_Header.TrackCnt);

		const int32 TrackCnt = NewRenderAnimData->_Header.TrackCnt;
		for (int32 i = 0; i < TrackCnt; i++)
		{
			const RKFTrack& OriginalTrack = OriginalRenderAnimData->_Tracks[i];
			const RKFTrack& NewTrack = NewRenderAnimData->_Tracks[i];

			SS_ASSERT(OriginalTrack._TrackName == NewTrack._TrackName);
			SS_ASSERT(OriginalTrack._TrackItems.GetSize() == NewTrack._TrackItems.GetSize());

			const int32 TrackItemCnt = OriginalTrack._TrackItems.GetSize();
			for (int32 j = 0; j < TrackItemCnt; j++)
			{
				const RKFTrackItem& OriginalTrackItem = OriginalTrack._TrackItems[j];
				const RKFTrackItem& NewTrackItem = OriginalTrack._TrackItems[j];

				SS_ASSERT(OriginalTrackItem._TimeRatio == NewTrackItem._TimeRatio);
				SS_ASSERT(OriginalTrackItem._Method == NewTrackItem._Method);
				SS_ASSERT(OriginalTrackItem._Padding == NewTrackItem._Padding);

				bool bResult = XMAlmostEqual(
					OriginalTrackItem._Transform.Scale.SimdVec, NewTrackItem._Transform.Scale.SimdVec);
				SS_ASSERT(bResult);
				bResult = XMAlmostEqual(
					OriginalTrackItem._Transform.Rotation.SimdVec, NewTrackItem._Transform.Rotation.SimdVec);
				SS_ASSERT(bResult);
				bResult = XMAlmostEqual(
					OriginalTrackItem._Transform.Position.SimdVec, NewTrackItem._Transform.Position.SimdVec);
				SS_ASSERT(bResult);
			}
		}
	}

	delete NewRenderAnimData;
}
