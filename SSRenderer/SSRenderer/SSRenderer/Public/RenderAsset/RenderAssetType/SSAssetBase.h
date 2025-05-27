#pragma once
#include "RenderAssetCommon/CommonDataType.h"
#include "SObject/Public/SObjHashCode.h"


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
	AssetInstanceReferencer(){ }
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
		default: ;
		}

		DEBUG_BREAK();
		return false;
	}
};

class SSAssetBase : public INoncopyable
{
private:
	EAssetType _assetType;
	SS::SHasherW _assetName;
	SS::SHasherW _assetPath;

public:
	SSAssetBase(EAssetType InAssetType, SS::SHasherW InAssetName, SS::SHasherW InAssetPath);
	SSAssetBase(EAssetType InAssetType);
	virtual ~SSAssetBase();

public:
	SS::SHasherW GetAssetName() const { return _assetName; }
	SS::SHasherW GetAssetPath() const { return _assetPath; }
	EAssetType GetAssetType() const { return _assetType; }
	int32 GetAssetInstanceReferenceCnt() const { return _AssetInstanceReferencers.GetSize(); }

public:
	void AddAssetReference(const AssetInstanceReferencer& Referencer);
	void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName);


private:
	SS::PooledList<AssetInstanceReferencer> _AssetInstanceReferencers;

	//TODO: virtual void Serialize() = 0;
};

