#include "AssetManagerBase.h"

#include "RenderAssetType/SSAssetBase.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"

AssetManagerBase::AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity)
	: _assetHashMap(AssetHashMapCapacity, AssetHashMapBucketCapacity)
{
}

AssetManagerBase::~AssetManagerBase()
{
}

void AssetManagerBase::AddToAssetPool(SSAssetBase* newAsset)
{
	SSAssetBase** foundModel = _assetHashMap.Find(newAsset->GetAssetName());
	if (foundModel != nullptr)
	{
		SS_ASSERT_MSG(false, L"이미 존재하는 에셋입니다.");
	}

	_assetHashMap.Add(newAsset->GetAssetName(), newAsset);
}

void AssetManagerBase::ReleaseAllAssets()
{
	for (SS::pair<SS::SHasherW, SSAssetBase*>& modelAssetItem : _assetHashMap)
	{
		delete modelAssetItem.second;
	}

	_assetHashMap.Clear();
}

SS::SHasherW AssetManagerBase::GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, const utf16* suffix) const
{
	SS::StringW newAssetName = fileName;
	newAssetName += L"/";
	newAssetName += nodeName;
	newAssetName += suffix;

	SS::SHasherW NewAssetNameHasher;

	SSAssetBase* FoundModel = FindAssetByName(newAssetName.C_Str());
	if (FoundModel == nullptr)
	{
		NewAssetNameHasher = newAssetName.C_Str();
		return NewAssetNameHasher;
	}

	int32 suffixNo = 1;

	do
	{
		newAssetName = fileName;
		newAssetName += L"/";
		newAssetName += nodeName;
		newAssetName += L"_";

		SS::StringW SuffixNoStr = IntToString(suffixNo++);
		newAssetName += SuffixNoStr;

		newAssetName += L".mdl";

		FoundModel = FindAssetByName(newAssetName.C_Str());

	} while (FoundModel != nullptr);


	NewAssetNameHasher = newAssetName.C_Str();
	return NewAssetNameHasher;
}

SSAssetBase* AssetManagerBase::FindAssetByName(SS::SHasherW InModelAssetName) const
{
	SSAssetBase* const* ppFoundModelAsset = _assetHashMap.Find(InModelAssetName);
	if (ppFoundModelAsset == nullptr)
	{
		return nullptr;
	}

	SSAssetBase* FoundModelAsset = *ppFoundModelAsset;

	return FoundModelAsset;
}
