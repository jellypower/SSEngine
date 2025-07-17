#pragma once
#include "RenderAssetType/RenderAssetCommon/CommonDataType.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"

class IAssetBase;

class IAssetManager : public INoncopyable
{
public:
	virtual IAssetBase* FindAssetByName(SS::SHasherW InModelAssetName, EAssetType InAssetType) const = 0;
	template<typename TAsset>
	TAsset* FindAssetByName(SS::SHasherW InAssetName) const
	{
		static_assert(std::derived_from<TAsset, IAssetBase>);
		return (TAsset*)FindAssetByName(InAssetName, TAsset::ThisAssetType);
	}

	virtual const SS::HashMap<SS::SHasherW, IAssetBase*>& GetAssetMap(EAssetType InAssetType) const = 0;
};
