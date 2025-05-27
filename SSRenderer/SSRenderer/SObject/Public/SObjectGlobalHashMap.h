#pragma once
#include "SObjectBase.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"

constexpr int32 SOBJECT_HASHMAP_HEADLIST_SIZE = 1024;
constexpr int32 SOBJECT_HASHMAP_BUCKET_SIZE = 1024 * 16;

class SObjectGlobalHashMap
{
private:
	volatile int64 CurNewObjectIdx = 1;
	SS::HashMap<SObjHashCode, SObjectBase*> _ObjectMapPool;


public:
	SObjectGlobalHashMap();

	SObjectBase* FindObject(SObjHashCode InHashCode) const;
	bool AnySObjectAlive() const;

	SObjHashCode InsertNewObject(SObjectBase* NewObject);
	void RemoveObject(SObjHashCode InHashCode);
};
