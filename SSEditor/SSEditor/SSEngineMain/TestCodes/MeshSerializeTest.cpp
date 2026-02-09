#include "pch.h"

#include "MeshSerializeTest.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"

#include "SSRenderer/Public/RenderAsset/IAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetSerializeFunctions.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"


void MeshSerializeTest(IRenderer* InRenderer, SS::SHasherW MeshToTest)
{
	IAssetManager* AssetManager = InRenderer->GetMutableAssetManager();

	IMeshAsset* MeshAsset = AssetManager->FindAssetByName<IMeshAsset>(MeshToTest);

	const MeshRawDataBase* MeshRawData = MeshAsset->GetMeshRawData();
	SS::PooledList<byte> Data;
	int WrittenBytes = AppendDataFromMeshAsset(Data, MeshRawData);
	SS_ASSERT(WrittenBytes > 0);


	MeshRawDataBase* MeshRawDataMutable = const_cast<MeshRawDataBase*>(MeshRawData);
	EMeshType MeshType = MeshRawDataMutable->GetMeshType();
	if (MeshType != EMeshType::Rigid && MeshType != EMeshType::Skinned)
	{
		SS_ASSERT(false);
		return;
	}

	MeshRawDataDefault* MeshRawDataDefaultMutable = static_cast<MeshRawDataDefault*>(MeshRawDataMutable);
	int ReadBytes = FillMeshAssetFromData(MeshRawDataDefaultMutable, Data);

	SS_ASSERT(WrittenBytes == ReadBytes);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();
}
