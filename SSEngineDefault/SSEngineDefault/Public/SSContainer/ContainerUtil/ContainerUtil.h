#pragma once
#include "SSEngineDefault/Public/SSContainer/PooledList.h"

template<typename ItemType, typename AllocatorType>
inline void ListPushBackUnique(SS::PooledList<ItemType, AllocatorType>& InPooledList, ItemType NewItem)
{
	for (ItemType Item : InPooledList)
	{
		if (Item == NewItem)
		{
			return;
		}
	}

	InPooledList.PushBack(NewItem);
}

template<typename ItemType, typename AllocatorType>
inline int64 ListFindItemIdx(const SS::PooledList<ItemType, AllocatorType>& InPooledList, ItemType FindingItem)
{
	int64 ListSize = InPooledList.GetSize();
	for (int64 i = 0; i < ListSize; i++)
	{
		if (InPooledList[i] == FindingItem)
		{
			return i;
		}
	}

	return INVALID_IDX;
}
