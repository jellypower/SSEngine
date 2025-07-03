#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "RenderAssetCommon/CommonDataType.h"

enum class EAssetInstanceReferenceType : uint8
{
	None,

	AssetName,
	ObjectHashCode,
};

struct AssetInstanceReferencer
{
	union
	{
		SObjHashCode ObjHashCode;
		SS::SHasherW AssetName;
	};
	EAssetInstanceReferenceType Type = EAssetInstanceReferenceType::None;

public:
	AssetInstanceReferencer() { }
	AssetInstanceReferencer(const AssetInstanceReferencer& rhs)
	{
		Type = rhs.Type;
		switch (Type)
		{
		case EAssetInstanceReferenceType::AssetName:
			AssetName = rhs.AssetName;
			break;
		case EAssetInstanceReferenceType::ObjectHashCode:
			ObjHashCode = rhs.ObjHashCode;
			break;
		}
	}

	bool operator==(const AssetInstanceReferencer& rhs) const
	{
		switch (Type)
		{
		case EAssetInstanceReferenceType::AssetName:
			return AssetName == rhs.AssetName;
		case EAssetInstanceReferenceType::ObjectHashCode:
			return ObjHashCode == rhs.ObjHashCode;
		default:;
		}

		DEBUG_BREAK();
		return false;
	}
};


class IAssetBase : public INoncopyable
{
protected:
	EAssetType _assetType = EAssetType::None;
	SS::SHasherW _assetName;
	SS::SHasherW _assetPath;

	SS::PooledList<AssetInstanceReferencer> _AssetInstanceReferencers;


public:
	SS::SHasherW GetAssetName() const { return _assetName; }
	SS::SHasherW GetAssetPath() const { return _assetPath; }
	EAssetType GetAssetType() const { return _assetType; }
	int32 GetAssetInstanceReferenceCnt() const { return _AssetInstanceReferencers.GetSize(); }


public:
	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) = 0;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) = 0;

	//TODO: virtual void Serialize() = 0;
};