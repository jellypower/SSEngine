#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/TestCodes/TestFunctions.h"

#include <unordered_map>


#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"
#include "TestClasses/TestCustomHeapAllocator.h"
#include "SSEngineDefault/Public/SSContainer/HashMap.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/SSContainer/CityHash.h"

void ContainerTest_PooledLinkedList()
{
	{
		// Test01
		{
			SS::PooledLinkedList<int32> IntList(100);
			for (int32 i = 0; i < 10; i++)
			{
				IntList.PushBack(i);
			}

			int32 a = 10;
		}

		// Test02
		{
			SS::PooledLinkedList<int32> IntList(100);
			const SS::PooledLinkedList<int32>& IntListConstRef = IntList;
			for (int32 j = 0; j < 100; j++)
			{
				for (int32 i = 0; i < 10; i++)
				{
					IntList.PushBack(i);
				}


				SS::PooledLinkedList<int32>::iterator iter = IntList.FindIteratorAt(3);
				IntList.InsertFront(iter, 33);
				IntList.InsertBack(iter, 33);


				SS::PooledLinkedList<int32>::iterator iter7 = IntList.FindIteratorAt(7);
				IntList.InsertFront(iter7, 77);
				IntList.InsertBack(iter7, 77);

				IntList.Erase(iter7);



				SS::PooledLinkedList<int32>::iterator iter10 = IntList.FindIteratorAt(10);
				IntList.Erase(iter10);


				IntList.Clear();


				for (int32 i = 0; i < 10; i++)
				{
					IntList.PushBack(i);
				}

			}
			int a = 0;
		}

		// Test03
		{
			SS::PooledLinkedList<SS::PooledList<int32>> ListList(100);

			for (int32 i = 0; i < 10; i++)
			{
				ListList.PushBack(SS::PooledList<int32>());
			}

			for (SS::PooledList<int32>& ListItem : ListList)
			{
				for (int32 i = 0; i < 10; i++)
				{
					ListItem.PushBack(i);
				}
			}

			ListList.Clear();

			for (int32 i = 0; i < 10; i++)
			{
				ListList.PushBack(SS::PooledList<int32>());
			}

			for (SS::PooledList<int32>& ListItem : ListList)
			{
				for (int32 i = 0; i < 10; i++)
				{
					ListItem.PushBack(i);
				}
			}

			int aa = 10;
		}

		// Test04
		{
			SS::PooledLinkedList<int32> IntList;
			for (int32 i = 0; i < 10; i++)
			{
				IntList.PushBack(i);
			}
			int32 aa = 10;
		}

		// Test05
		{
			SS::PooledLinkedList<int32> IntList1;
			SS::PooledLinkedList<int32> IntList2;
			for (int32 i = 0; i < 10; i++)
			{
				IntList2.PushBack(i);
			}

			IntList1 = IntList2;
			for (int32 i = 0; i < 10; i++)
			{
				int32 value1 = IntList1.FindAt(i);
				int32 value2 = IntList2.FindAt(i);

				SS_ASSERT(value1 == value2);
			}

			IntList2.Clear();
			IntList2.PushBack(5);
			for (int32 i = 0; i < 10; i++)
			{
				int32 value = IntList1.FindAt(i);
				SS_ASSERT(value == i);
			}

			IntList1.Clear();
			IntList2.Clear();
			for (int32 i = 0; i < 10; i++)
			{
				IntList2.PushBack(i);
			}

			IntList1 = SS::move(IntList2);
			for (int32 i = 0; i < 10; i++)
			{
				int32 value = IntList1.FindAt(i);
				SS_ASSERT(value == i);
			}
			SS_ASSERT(IntList2.GetSize() == 0);
		}

		int a = 10;
	}
}

void ContainerTest_HashMap()
{
	struct Hasher
	{
		std::size_t operator()(const SS::StringW& InValue) const
		{
			return HashValue(InValue);
		}
	};


	constexpr int32 ITER_CNT = 10000;

	SS::PooledList<SS::StringW> StrList;

	for (int32 i = 0; i < ITER_CNT; i++)
	{
		utf16 TempStr[2000];
		swprintf_s(TempStr, sizeof(TempStr)/sizeof(utf16), L"MyString: %d", i);

		StrList.PushBack(TempStr);
	}


	constexpr int32 RESERVE_SIZE = ITER_CNT;

	std::unordered_map<SS::StringW, int32, Hasher> UMap;
	UMap.reserve(RESERVE_SIZE * 2);

	SS::HashMap<SS::StringW, int32> HashMap(RESERVE_SIZE, RESERVE_SIZE);


	// Test01
	{
		uint64 freqStart, freqEnd;

		freqStart = GetPerofrmanceCounter();
		for (int32 i = 0; i < ITER_CNT; i++)
		{
			UMap.insert(std::make_pair(StrList[i], i));
		}
		freqEnd = GetPerofrmanceCounter();

		uint64 eTime1 = freqEnd - freqStart;


		freqStart = GetPerofrmanceCounter();
		for (int32 i = 0; i < ITER_CNT; i++)
		{
			const SS::StringW StrItem = StrList[i];
			HashMap.Add(StrItem, i);
		}
		freqEnd = GetPerofrmanceCounter();

		int64 eTime2 = freqEnd - freqStart;

		int64 TimeDiff = eTime1 - eTime2;
		int a = 0;
	}


	// Test02
	{

		for (int32 i = 0; i < ITER_CNT; i++)
		{
			utf16 TempStr[2000];
			swprintf_s(TempStr, sizeof(TempStr)/sizeof(utf16), L"MyString: %d", i);

			int32* FoundItem = HashMap.Find(TempStr);

			if (FoundItem == nullptr)
			{
				SS_ASSERT(false);
				return;
			}

			int32 FoundValue = *FoundItem;
			SS_ASSERT(FoundValue == i);
		}
	}
}

void CustomHeapAllocatorTest()
{
	{
		TestCustomHeapAllocator Allocator(100, 5);

		{
			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(5);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(15);
			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(20);
			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(70);

			Allocator.ReleaseChunk(Desc2);
			Allocator.ReleaseChunk(Desc4);
			Allocator.ReleaseChunk(Desc3);
			Allocator.ReleaseChunk(Desc1);
			Allocator.ReleaseChunk(Desc5);
			Allocator.ReleaseDefaultPages();
		}

		{
			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(5);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(15);
			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(20);

			Allocator.ReleaseChunk(Desc2);
			Allocator.ReleaseChunk(Desc3);
			Allocator.ReleaseChunk(Desc1);
			Allocator.ReleaseChunk(Desc4);


			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(70);
			Allocator.ReleaseChunk(Desc5);

			SS_ASSERT(Desc5.DefaultPageIdx == 0);

			Allocator.ReleaseDefaultPages();
		}

		{
			for (int32 i = 0; i > 10; i++)
			{
				AllocatedChunkHeader Desc = Allocator.AllocChunk(50);
				Allocator.ReleaseChunk(Desc);
			}

			SS_ASSERT(Allocator.IsAnyChunkInUse() == false);
			Allocator.ReleaseDefaultPages();
		}


		{
			AllocatedChunkHeader Desc50 = Allocator.AllocChunk(50);
			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(10);

			Allocator.ReleaseChunk(Desc2);
			Allocator.ReleaseChunk(Desc4);
			Allocator.ReleaseChunk(Desc3);

			AllocatedChunkHeader NewDesc = Allocator.AllocChunk(30);
			SS_ASSERT(NewDesc.ChunkOffset == 60);
			SS_ASSERT(NewDesc.Size == 30);

			Allocator.ReleaseChunk(Desc5);
			Allocator.ReleaseChunk(Desc1);
			Allocator.ReleaseChunk(Desc50);

			SS_ASSERT(Allocator.IsAnyChunkInUse());
			Allocator.ReleaseChunk(NewDesc);

			Allocator.ReleaseDefaultPages();
		}


		{
			Allocator.ReserveDefaultPage(20);
			Allocator.ReleaseDefaultPages();
		}
	}

	{
		TestCustomHeapAllocator Allocator(1000, 10);

		{
			AllocatedChunkHeader Desc300 = Allocator.AllocChunk(300);

			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(20);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(30);
			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(40);
			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(50);
			AllocatedChunkHeader Desc6 = Allocator.AllocChunk(60);
			AllocatedChunkHeader Desc7 = Allocator.AllocChunk(70);
			AllocatedChunkHeader Desc8 = Allocator.AllocChunk(80);
			AllocatedChunkHeader Desc9 = Allocator.AllocChunk(90);
			AllocatedChunkHeader Desc10 = Allocator.AllocChunk(100);

			Allocator.ReleaseChunk(Desc2);
			Allocator.ReleaseChunk(Desc4);
			Allocator.ReleaseChunk(Desc6);
			Allocator.ReleaseChunk(Desc8);
			Allocator.ReleaseChunk(Desc10);

			Allocator.ReleaseChunk(Desc300);

			Allocator.ReleaseChunk(Desc3);
			Allocator.ReleaseChunk(Desc5);
			Allocator.ReleaseChunk(Desc7);
			Allocator.ReleaseChunk(Desc9);
			Allocator.ReleaseChunk(Desc1);

			Allocator.ReleaseDefaultPages();
		}

		{
			AllocatedChunkHeader Desc300 = Allocator.AllocChunk(300);

			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(20);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(30);
			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(40);
			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(50);
			AllocatedChunkHeader Desc6 = Allocator.AllocChunk(60);
			AllocatedChunkHeader Desc7 = Allocator.AllocChunk(70);
			AllocatedChunkHeader Desc8 = Allocator.AllocChunk(80);
			AllocatedChunkHeader Desc9 = Allocator.AllocChunk(90);
			AllocatedChunkHeader Desc10 = Allocator.AllocChunk(100);

			Allocator.ReleaseChunk(Desc2);
			Allocator.ReleaseChunk(Desc4);
			Allocator.ReleaseChunk(Desc6);
			Allocator.ReleaseChunk(Desc8);
			Allocator.ReleaseChunk(Desc10);

			Allocator.ReleaseChunk(Desc300);

			Allocator.ReleaseChunk(Desc9);
			Allocator.ReleaseChunk(Desc5);
			Allocator.ReleaseChunk(Desc7);
			Allocator.ReleaseChunk(Desc3);
			Allocator.ReleaseChunk(Desc1);

			Allocator.ReleaseDefaultPages();
		}



	}

	{
		TestCustomHeapAllocator Allocator(50, 10);

		{
			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc2 = Allocator.AllocChunk(10);
			AllocatedChunkHeader Desc3 = Allocator.AllocChunk(10);

			Allocator.ReleaseChunk(Desc1);

			AllocatedChunkHeader Desc4 = Allocator.AllocChunk(10);
			SS_ASSERT(Desc4.ChunkOffset == 0);

			AllocatedChunkHeader Desc5 = Allocator.AllocChunk(20);
			SS_ASSERT(Desc5.ChunkOffset == 30 && Desc5.Size == 20);

			AllocatedChunkHeader Desc6 = Allocator.AllocChunk(10);
			SS_ASSERT(Desc6.DefaultPageIdx == 1);

			Allocator.ReleaseChunk(Desc2);
			AllocatedChunkHeader Desc7 = Allocator.AllocChunk(10);
			SS_ASSERT(Desc7.ChunkOffset == 10, Desc7.Size == 10);

			Allocator.ReleaseChunk(Desc5);
			Allocator.ReleaseChunk(Desc3);
			Allocator.ReleaseChunk(Desc7);
			Allocator.ReleaseChunk(Desc4);
			Allocator.ReleaseChunk(Desc6);

			Allocator.ReleaseDefaultPages();
		}
	}

	{
		TestCustomHeapAllocator Allocator(50, 10);

		{
			AllocatedChunkHeader Desc1 = Allocator.AllocChunk(1);
			SS_ASSERT(Desc1.Size == 10);

			Allocator.ReleaseChunk(Desc1);
		}

		Allocator.ReleaseDefaultPages();
	}

}

void SHasherPoolTest()
{
	IHasherPool* PoolForTest = CreateHasherPool(10000);

	for (int32 i=0;i<5000;i++)
	{

		utf16 TempStr[500];
		swprintf_s(TempStr, sizeof(TempStr) / sizeof(utf16), L"MyString: %d", i);
		utf16 LoweredStr[500];

		LowerStr(TempStr, LoweredStr);

		int32 strLen = wcslen(LoweredStr);
		int32 HashedValue = CityHash32(reinterpret_cast<const char*>(LoweredStr), strLen * (sizeof(utf16) / sizeof(char)));

		PoolForTest->FindOrAddHasherValue(LoweredStr, strLen, HashedValue);
	}

	for (int32 i = 0; i < 5000; i++)
	{

		utf16 TempStr[500];
		swprintf_s(TempStr, sizeof(TempStr) / sizeof(utf16), L"MyString: %d", i);
		utf16 LoweredStr[500];

		LowerStr(TempStr, LoweredStr);

		uint32 strLen = wcslen(LoweredStr);
		uint32 HashedValue = CityHash32(reinterpret_cast<const char*>(LoweredStr), strLen * (sizeof(utf16) / sizeof(char)));

		uint64 Value1 = PoolForTest->FindOrAddHasherValue(LoweredStr, strLen, HashedValue);
		uint64 Value2 = PoolForTest->FindOrAddHasherValue(LoweredStr, strLen, HashedValue);

		wcscpy(TempStr, L"MyStr");
		LowerStr(TempStr, LoweredStr);
		strLen = wcslen(LoweredStr);
		HashedValue = CityHash32(reinterpret_cast<const char*>(LoweredStr), strLen * (sizeof(utf16) / sizeof(char)));
		uint64 Value3 = PoolForTest->FindOrAddHasherValue(LoweredStr, 5, HashedValue);

		SS_ASSERT(Value1 == Value2);
		SS_ASSERT(Value1 != Value3);
	}

	delete PoolForTest;
}

void SHasherTest()
{
	SS::PooledList<int32> list;
	list.PushBack(1);
	list.PushBack(1);
	list.PushBack(1);

	for (int32 i = 0; i < 5000; i++)
	{
		utf16 TempStr[500];
		swprintf_s(TempStr, sizeof(TempStr) / sizeof(utf16), L"MyString: %d", i);

		SS::SHasherW Hasher1 = SS::SHasherW(TempStr);
		SS::SHasherW Hasher2 = SS::SHasherW(TempStr);

		SS_ASSERT(Hasher1 == Hasher2);
	}
}
