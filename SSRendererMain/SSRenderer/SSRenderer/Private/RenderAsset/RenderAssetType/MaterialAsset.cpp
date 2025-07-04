#include "MaterialAsset.h"

MaterialAsset::MaterialAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	_assetName = InAssetName;
	_assetPath = InAssetPath;
}

EAssetType MaterialAsset::GetAssetType() const
{
	return EAssetType::Material;
}

void MaterialAsset::AddAssetReference(const AssetInstanceReferencer& Referencer)
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

	// TODO: AssetCount가 0에서 올라오면 본인이 레퍼런스하고있는 에셋들에게 레프카운트 올려주기
}

void MaterialAsset::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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

	// TODO: AssetCount가 0으로 떨어지면 본인이 레퍼런스하고있는 에셋들에게 레프카운트 올려주기
}
