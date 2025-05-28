#include "SObject/Public/SObjectGlobalHashMap.h"

#include <Windows.h>

SObjectGlobalHashMap::SObjectGlobalHashMap()
	: _ObjectMapPool(SOBJECT_HASHMAP_HEADLIST_SIZE, SOBJECT_HASHMAP_BUCKET_SIZE)
{

}

SObjectBase* SObjectGlobalHashMap::FindObject(SObjHashCode InHashCode) const
{
	SObjectBase* const* ppObject = _ObjectMapPool.Find(InHashCode);
	if (ppObject == nullptr)
	{
		return nullptr;
	}

	return *ppObject;
}

bool SObjectGlobalHashMap::AnySObjectAlive() const
{
	for (const SS::pair<SObjHashCode, SObjectBase*>& ObjectItem : _ObjectMapPool)
	{
		if (ObjectItem.second != nullptr)
		{
			return true;
		}
	}
	
	return false;
}

SObjHashCode SObjectGlobalHashMap::InsertNewObject(SObjectBase* NewObject)
{
	SObjHashCode InsertedObjectIdx = SObjHashCode(CurNewObjectIdx);
	_ObjectMapPool.Add(InsertedObjectIdx, NewObject);
	InterlockedIncrement64(&CurNewObjectIdx);
	return InsertedObjectIdx;
}

void SObjectGlobalHashMap::RemoveObject(SObjHashCode InHashCode)
{
	SObjectBase** ppObject = _ObjectMapPool.Find(InHashCode);
	if (ppObject == nullptr)
	{
		SS_ASSERT_MSG(false, L"There must be ");
		return;
	}

	*ppObject = nullptr;
}
