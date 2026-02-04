#include "AssetManagerBase.h"

#include "RenderAssetType/MaterialAsset.h"
#include "RenderAssetType/MeshAsset.h"
#include "RenderAssetType/ModelAsset.h"
#include "RenderAssetType/ModelCombinationAsset.h"
#include "RenderAssetType/RenderAnimAsset.h"
#include "RenderAssetType/TextureAsset.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IMeshAsset.h"



AssetManagerBase::AssetManagerBase(int32 AssetHashMapCapacity, int32 AssetHashMapBucketCapacity):
	_assetHashMap{
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

ITextureAssetMutable* AssetManagerBase::CreateEmptyTextureAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath, ETextureType InType)
{
	return DBG_NEW TextureAsset(InDBNameSpace, InAssetName, InAssetPath, InType);
}

IMeshAssetMutable* AssetManagerBase::CreateEmptyMeshAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	return DBG_NEW MeshAsset(InDBNameSpace, InAssetName, InAssetName);
}

IModelAssetMutable* AssetManagerBase::CreateEmptyModelAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	return DBG_NEW ModelAsset(InDBNameSpace, InAssetName, InAssetPath);
}

IModelCombinationAssetMutable* AssetManagerBase::CreateEmptyModelCombinationAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath, int32 ReservedChildCnt)
{
	return DBG_NEW ModelCombinationAsset(InDBNameSpace, InAssetName, InAssetPath, ReservedChildCnt);
}

IMaterialAssetMutable* AssetManagerBase::CreateEmptyMaterialAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName, SS::SHasherW InAssetPath)
{
	return DBG_NEW MaterialAsset(InDBNameSpace, InAssetName, InAssetPath);
}

IRenderAnimAssetMutable* AssetManagerBase::CreateEmptyRenderAnimAsset(SS::SHasherW InDBNameSpace, SS::SHasherW InAssetName,
	SS::SHasherW InAssetPath)
{
	return DBG_NEW RenderAnimAsset(InDBNameSpace, InAssetName, InAssetPath);
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

		newAssetName += GetAssetSuffix(InAssetType);

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

const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetManagerBase::GetAssetMap(EAssetType InAssetType) const
{
	return _assetHashMap[(int32)InAssetType];
}
