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
	newAsset->BindAssetManager(this);
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
	return DBG_NEW MeshAsset(InDBNameSpace, InAssetName, InAssetPath);
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

IAssetBase* AssetManagerBase::FindAssetByName(SS::SHasherW InAssetName, EAssetType InAssetType) const
{
	int32 i32InAssetName = (int32)InAssetType;

	if (i32InAssetName <= (int32)EAssetType::None || i32InAssetName >= (int32)EAssetType::Count)
	{
		SS_ASSERT(false);
		return nullptr;
	}

	if (InAssetName.IsEmpty())
	{
		SS_ASSERT(false);
		return nullptr;
	}

	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetMapOfType =
		_assetHashMap[(int32)InAssetType];

	IAssetBase* const* ppFoundModelAsset = AssetMapOfType.Find(InAssetName);
	if (ppFoundModelAsset == nullptr)
	{
		return nullptr;
	}

	IAssetBase* FoundModelAsset = *ppFoundModelAsset;
	
	return FoundModelAsset;
}

bool AssetManagerBase::AddAssetReferencer(SS::SHasherW InAssetName, EAssetType InAssetType,
	const AssetInstanceReferencer& Referencer)
{
	IAssetBase* FoundAsset = FindAssetByName(InAssetName, InAssetType);
	if (FoundAsset == nullptr)
	{
		SS_ASSERT(false);
		return false;
	}

	FoundAsset->AddAssetReference(Referencer);
	return true;
}

bool AssetManagerBase::RemoveAssetReferencer(SS::SHasherW InAssetName, EAssetType InAssetType,
	const AssetInstanceReferencer& Referencer)
{
	IAssetBase* FoundAsset = FindAssetByName(InAssetName, InAssetType);
	if (FoundAsset == nullptr)
	{
		SS_ASSERT(false);
		return false;
	}

	FoundAsset->RemoveAssetReference(Referencer);
	return true;
}

const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetManagerBase::GetAssetMap(EAssetType InAssetType) const
{
	return _assetHashMap[(int32)InAssetType];
}

void AssetManagerBase::FindAssetsOfNamespace(SS::PooledList<IAssetBase*>& AssetListToFill, SS::SHasherW Namespace,
	EAssetType InAssetType) const
{
	const SS::HashMap<SS::SHasherW, IAssetBase*>& AssetMap = _assetHashMap[(int32)InAssetType];


	for (const SS::pair<SS::SHasherW, IAssetBase*>& AssetPairItem : AssetMap)
	{
		IAssetBase* AssetItem = AssetPairItem.second;
		if (AssetItem->GetDBNameSpace() == Namespace)
		{
			AssetListToFill.PushBack(AssetItem);
		}
	}
}
