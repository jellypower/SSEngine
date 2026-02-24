#include "pch.h"

#include "MeshSerializeTest.h"

#include <SSRenderer/Public/RenderAsset/CommonRenderAsset/CRAN.h>

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IModelCombinationAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
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
