#include "SSAssetBase.h"


SSAssetBase::SSAssetBase(EAssetType InAssetType, SS::SHasherW InAssetName, SS::SHasherW  InAssetPath)
	: _assetType(InAssetType), _assetName(InAssetName), _assetPath(InAssetPath)
{
}

SSAssetBase::SSAssetBase(EAssetType InAssetType)
	: _assetType(InAssetType), _assetName(SS::SHasherW::Empty), _assetPath(SS::SHasherW::Empty)
{
}

SSAssetBase::~SSAssetBase()
{
}

void SSAssetBase::AddAssetReference(const AssetInstanceReferencer& Referencer)
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
}

void SSAssetBase::RemoveAssetReference(const AssetInstanceReferencer& ReferencerName)
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
}