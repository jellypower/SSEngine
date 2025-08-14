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
