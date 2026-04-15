#pragma once
#include "SObject/Public/SObjHashCode.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "RenderAssetCommon/CommonDataType.h"

#include <ctime>

class IAssetManager;

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
	SS::SHasherW _DBNameSpace;
	SS::SHasherW _assetName;
	SS::SHasherW _assetPath;
	time_t _LastUpdateTime = 0;

	IAssetManager* _BoundAssetManager = nullptr;

	SS::PooledList<AssetInstanceReferencer> _AssetInstanceReferencers;


public:
	AssetInstanceReferencer MakeThisAssetReferencer()
	{
		AssetInstanceReferencer ThisAssetReferencer;
		ThisAssetReferencer.Type = EAssetInstanceReferenceType::AssetName;
		ThisAssetReferencer.AssetName = _assetName;
		return ThisAssetReferencer;
	}

	SS::SHasherW GetDBNameSpace() const { return _DBNameSpace; }
	SS::SHasherW GetAssetName() const { return _assetName; }
	SS::SHasherW GetAssetPath() const { return _assetPath; }
	time_t GetLastUpdateTime() const { return _LastUpdateTime; }

	int32 GetAssetInstanceReferenceCnt() const { return _AssetInstanceReferencers.GetSize(); }

public:
	void MarkAsUpdated(){ time(&_LastUpdateTime); }
	void SetAssetPathXXX(SS::SHasherW InPath) { _assetPath = InPath; }

public:
	virtual EAssetType GetAssetType() const = 0;

	virtual void AddAssetReference(const AssetInstanceReferencer& Referencer) = 0;
	virtual void RemoveAssetReference(const AssetInstanceReferencer& ReferencerName) = 0;

	virtual void BindAssetManager(IAssetManager* InAssetManager) = 0;
};