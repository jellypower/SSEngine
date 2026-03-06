#pragma once
#include "RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

struct AssetInstanceReferencer;
class IAssetBase;

class IAssetManager : public INoncopyable
{
public:
	virtual const SS::HashMap<SS::SHasherW, IAssetBase*>& GetAssetMap(EAssetType InAssetType) const = 0;
	virtual void FindAssetsOfNamespace(SS::PooledList<IAssetBase*>& AssetListToFill, SS::SHasherW Namespace, EAssetType AssetType) const = 0;
	virtual void FindAssetsEditSince(SS::PooledList<IAssetBase*>& OutList, SS::SHasherW Namespace, EAssetType AssetType, time_t Since) const = 0;

	virtual IAssetBase* FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const = 0;
	template<typename TAsset>
	TAsset* FindAssetByName(SS::SHasherW InAssetName) const
	{
		static_assert(std::derived_from<TAsset, IAssetBase>);
		return (TAsset*)FindAssetByName(InAssetName, TAsset::ThisAssetType);
	}

public:
	virtual bool AddAssetReferencer(
		SS::SHasherW InAssetName, 
		EAssetType InAssetType, 
		const AssetInstanceReferencer& Referencer) = 0;

	virtual bool RemoveAssetReferencer(
		SS::SHasherW InAssetName,
		EAssetType InAssetType,
		const AssetInstanceReferencer& Referencer) = 0;
};
