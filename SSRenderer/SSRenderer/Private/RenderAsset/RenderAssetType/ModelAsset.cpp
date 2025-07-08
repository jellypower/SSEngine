#include "ModelAsset.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"

ModelAsset::ModelAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType ModelAsset::GetAssetType() const
{
	return ThisAssetType;
}

void ModelAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
{
	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == Referencer)
		{
			SS_ASSERT_MSG(false, L"Reference already exists.");
			return;
		}
	}

	int32 PrevReferencerCnt = _AssetInstanceReferencers.GetSize();

	_AssetInstanceReferencers.PushBack(Referencer);

	if (PrevReferencerCnt == 0)
	{
		AssetInstanceReferencer ThisReferencer;
		ThisReferencer.Type = EAssetInstanceReferenceType::AssetName;
		ThisReferencer.AssetName = GetAssetName();
		_MeshAsset->AddAssetReference(ThisReferencer);
		// TODO: Material 레프카운트 올려주기	
	}

}

void ModelAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
{
	bool bReferencerEverRemoved = false;

	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == ReferencerName)
		{
			_AssetInstanceReferencers.RemoveAtAndFillLast(i);
			bReferencerEverRemoved = true;
			break;
		}
	}

	if (bReferencerEverRemoved == false)
	{
		SS_ASSERT_MSG(false, L"Reference does not exist.");
		return;
	}

	int32 ReferencerCnt = _AssetInstanceReferencers.GetSize();
	if (ReferencerCnt == 0)
	{
		AssetInstanceReferencer ThisReferencer;
		ThisReferencer.Type = EAssetInstanceReferenceType::AssetName;
		ThisReferencer.AssetName = GetAssetName();
		_MeshAsset->RemoveAssetReference(ThisReferencer);

		// TODO: AssetCount가 0으로 떨어지면 메테리얼 레퍼런스도 내려주기
	}


}

int32 ModelAsset::GetSubMeshCnt() const
{
	if (_MeshAsset == nullptr)
	{
		return 0;
	}

	const MeshRawDataBase* MeshRawData = _MeshAsset->GetMeshRawData();
	if (MeshRawData == nullptr)
	{
		return 0;
	}

	if (MeshRawData->_MeshType == EMeshType::Rigid)
	{
		MeshRawDataDefault* DefaultMeshRawData = (MeshRawDataDefault*)MeshRawData;
		return DefaultMeshRawData->_subMeshCnt;
	}
	else
	{
		SS_ASSERT(false);
		return 0;
	}
}

void ModelAsset::SetMesh(IMeshAsset* InMeshAsset)
{
	_MeshAsset = InMeshAsset;
}

void ModelAsset::SetMaterial(IMaterialAsset* InMaterialAsset, int32 InMaterialIdx)
{
	if (InMaterialIdx >= SUBMESH_COUNT_MAX)
	{
		SS_ASSERT(false);
		return;
	}

	if (_MeshAsset == nullptr)
	{
		SS_ASSERT(false);
		return;
	}

	const MeshRawDataBase* RawData = _MeshAsset->GetMeshRawData();


	if (RawData->_MeshType == EMeshType::Rigid)
	{
		MeshRawDataDefault* DefaultRawData = (MeshRawDataDefault*)RawData;
		if (DefaultRawData->_subMeshCnt <= InMaterialIdx)
		{
			SS_ASSERT(false);
			return;
		}

		_MaterialAssets[InMaterialIdx] = InMaterialAsset;
		return;
	}

	SS_ASSERT(false);
	return;
}