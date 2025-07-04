#include "AssetManagerBase.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"

const char* AssetManagerBase::GetAssetSuffix(EAssetType InAssetType)
{
	switch (InAssetType)
	{
	case EAssetType::Mesh: return ".mesh";
	case EAssetType::Material: return ".mat";
	case EAssetType::Model: return ".mdl";
	case EAssetType::ModelCombination: return ".mdlc";
	case EAssetType::Texture: return ".tex";
	case EAssetType::Skeleton: return ".skl";
	case EAssetType::SkeletonAnim: return ".sanim";
		break;
	}

	SS_ASSERT(false);
	return nullptr;
}

AssetManagerBase::AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity):
	_assetHashMap{
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
		SS::HashMap<SS::SHasherW, IAssetBase*>(AssetHashMapCapacity, AssetHashMapBucketCapacity),
	}
{
}

AssetManagerBase::~AssetManagerBase()
{
}

void AssetManagerBase::AddToAssetPool(IAssetBase* newAsset)
{
	SS::HashMap<SS::SHasherW, IAssetBase*>& AssetMapOfType = 
		_assetHashMap[(int32)newAsset->GetAssetType()];

	IAssetBase ** FoundAsset = AssetMapOfType.Find(newAsset->GetAssetName());
	if (FoundAsset != nullptr)
	{
		SS_ASSERT_MSG(false, L"이미 존재하는 에셋입니다.");
	}

	AssetMapOfType.Add(newAsset->GetAssetName(), newAsset);
}

void AssetManagerBase::ReleaseAllAssets()
{
	SS::HashMap<SS::SHasherW, IAssetBase*>& MeshAssetMap = _assetHashMap[(int32)EAssetType::Mesh];
	for (SS::pair<SS::SHasherW, IAssetBase*>& AssetItemPair : MeshAssetMap)
	{
		IMeshAsset* MeshAsset = (IMeshAsset*)AssetItemPair.second;
		MeshAsset->ReleaseSystemData();
		MeshAsset->ReleaseGALData();
		delete MeshAsset;
	}
	MeshAssetMap.Clear();

	for (int32 AssetTypeIdx = 0; AssetTypeIdx < (int32)EAssetType::Count; AssetTypeIdx++)
	{
		SS::HashMap<SS::SHasherW, IAssetBase*>& AssetMap = _assetHashMap[AssetTypeIdx];

		for (SS::pair<SS::SHasherW, IAssetBase*>& AssetItem : AssetMap)
		{
			delete AssetItem.second;
		}

		AssetMap.Clear();
	}
}

SS::SHasherW AssetManagerBase::GenerateAssetName(const SS::StringW& fileName, const SS::StringW& nodeName, EAssetType InAssetType) const
{
	SS::StringW newAssetName = fileName;
	newAssetName += L"/";
	newAssetName += nodeName;
	newAssetName += GetAssetSuffix(InAssetType);

	SS::SHasherW NewAssetNameHasher;

	IAssetBase* FoundAsset = FindAssetByName(newAssetName.C_Str(), InAssetType);
	if (FoundAsset == nullptr)
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

		newAssetName += L".mdl"; // TODO: 에셋 이름 생성부분이 뭔가 수정이 필요해보임

		FoundAsset = FindAssetByName(newAssetName.C_Str(), InAssetType);

	} while (FoundAsset != nullptr);


	NewAssetNameHasher = newAssetName.C_Str();
	return NewAssetNameHasher;
}

IAssetBase* AssetManagerBase::FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const
{
	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetMapOfType =
		_assetHashMap[(int32)InAssetType];

	IAssetBase* const* ppFoundModelAsset = AssetMapOfType.Find(InModelAssetName);
	if (ppFoundModelAsset == nullptr)
	{
		return nullptr;
	}

	IAssetBase* FoundModelAsset = *ppFoundModelAsset;
	
	return FoundModelAsset;
}
