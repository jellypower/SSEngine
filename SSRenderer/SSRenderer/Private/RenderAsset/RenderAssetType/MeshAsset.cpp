#include "MeshAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"

MeshAsset::MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType MeshAsset::GetAssetType() const
{
	return ThisAssetType;
}

void MeshAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
{
	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == Referencer)
		{
			SS_ASSERT_MSG(false, L"Reference already exists.");
			return;
		}
	}

	_AssetInstanceReferencers.PushBack(Referencer);

	// TODO: RefCount가 0에서 올라올 때 해제
}

void MeshAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
{
	for (int32 i = 0; i < _AssetInstanceReferencers.GetSize(); i++)
	{
		if (_AssetInstanceReferencers[i] == ReferencerName)
		{
			_AssetInstanceReferencers.RemoveAtAndFillLast(i);
			return;
		}
	}

	SS_ASSERT_MSG(false, L"Reference does not exist.");

	// TODO: RefCount가 0으로 떨어질 때 해제
}

void MeshAsset::ReleaseSystemData()
{
	delete _MeshRawData;
	_MeshRawData = nullptr;
}

void MeshAsset::ReleaseGALData()
{
	delete _GALMeshAsset;
	_GALMeshAsset = nullptr;
}
