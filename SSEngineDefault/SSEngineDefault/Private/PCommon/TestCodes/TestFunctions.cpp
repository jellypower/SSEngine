#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/TestCodes/TestFunctions.h"

#include <ctime>
#include <unordered_map>


#include "SSEngineDefault/Public/SSDirectXMathCustom.h"
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
		swprintf_s(TempStr, sizeof(TempStr) / sizeof(utf16), L"MyString: %d", i);

		StrList.PushBack(TempStr);
	}


	constexpr int32 RESERVE_SIZE = ITER_CNT;

	std::unordered_map<SS::StringW, int32, Hasher> UMap;
	UMap.reserve(RESERVE_SIZE * 2);

	SS::HashMap<SS::StringW, int32> HashMap(RESERVE_SIZE, RESERVE_SIZE);


	// Test01: 속도 테스트 
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


	// Test02: 찾기 지우기 테스트
	{

		for (int32 i = 0; i < ITER_CNT; i++)
		{
			utf16 TempStr[2000];
			swprintf_s(TempStr, sizeof(TempStr) / sizeof(utf16), L"MyString: %d", i);

			int32* FoundItem = HashMap.Find(TempStr);

			if (FoundItem == nullptr)
			{
				SS_ASSERT(false);
				return;
			}

			int32 FoundValue = *FoundItem;
			SS_ASSERT(FoundValue == i);

			bool bRemoveSuccess = HashMap.Remove(TempStr);
			SS_ASSERT(bRemoveSuccess);
		}

		SS_ASSERT(HashMap.GetCnt() == 0);
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
		TestCustomHeapAllocator Allocator(1000, 10, 1);

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
		TestCustomHeapAllocator Allocator(50, 10, 1);

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

void StringTest()
{
	SS::StringW TestStr1 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L" 쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	TestStr1.Replace(L"나", L"호날두");
	SS::StringW TestStr2 = L"너는 호날두를 존중해야 한다. 호날두는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며"
		L" 2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이호날두 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 호날두이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며 "
		L"내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고 "
		L"쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간 "
		L"이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L" 쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	TestStr1.Replace(L"고", L"고라니");
	TestStr2 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고라니 우승을 차지했고라니 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고라니 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고라니임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L"라니 쫓아냈지만, 내가 세계 최고라니이고라니 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = L"호날두";
	TestStr1.Replace(L"호날두", L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고라니 우승을 차지했고라니 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고라니 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고라니임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L"라니 쫓아냈지만, 내가 세계 최고라니이고라니 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.");
	TestStr2 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고라니 우승을 차지했고라니 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고라니 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고라니임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L"라니 쫓아냈지만, 내가 세계 최고라니이고라니 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	SS_ASSERT(TestStr1 == TestStr2);




	TestStr1 = "hhhhab";
	TestStr1.Replace(L"hh", L"Hello");
	TestStr2 = "HelloHelloab";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = "abcdefghijklmnop";
	TestStr1.Replace(L"h", L"Hello");
	TestStr2 = "abcdefgHelloijklmnop";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = "hhab";
	TestStr1.Replace(L"hh", L"Hello");
	TestStr2 = "Helloab";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = "h h h h h h h h ";
	TestStr1.Replace(L" ", L"ello");
	TestStr2 = "hellohellohellohellohellohellohellohello";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = "Hello Hello Hello Hello Hello";
	TestStr1.Replace(L"Hello", L"h");
	TestStr2 = "h h h h h";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = "안녕하세요";
	TestStr1.Replace(L"세요", L"시와요");
	TestStr2 = "안녕하시와요";
	SS_ASSERT(TestStr1 == TestStr2);


	TestStr1 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고라니 우승을 차지했고라니 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고라니 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고라니임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L"라니 쫓아냈지만, 내가 세계 최고라니이고라니 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	TestStr1.Replace(L"고라니", L"고");
	TestStr2 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
		L"2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
		L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
		L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며"
		L" 내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
		L" 쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
		L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
		L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = L"안 녕 하 세 요";
	TestStr1.Replace(L" ", L"");
	TestStr2 = L"안녕하세요";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = L"I think Ruth's ferret is cuter than your dog!";
	TestStr1.Replace(L"Ruth's", L"my");
	TestStr1.Replace(L"ferret", L"dog");
	TestStr2 = L"I think my dog is cuter than your dog!";
	SS_ASSERT(TestStr1 == TestStr2);


	TestStr1 = L"  TEST  ";
	TestStr1.Replace(L" ", L"_");
	TestStr2 = L"__TEST__";
	SS_ASSERT(TestStr1 == TestStr2);

	TestStr1 = L"  TEST  ";
	TestStr1.Replace(L"  ", L"__");
	TestStr2 = L"__TEST__";
	SS_ASSERT(TestStr1 == TestStr2);

	if (_CrtCheckMemory() == false) SS_INTERRUPT();
	int a = 0;
}

void DirectXMathTest()
{
	{
		XMVECTOR e1;
		e1.m128_f32[0] = XM_PIDIV2;
		e1.m128_f32[1] = 0;
		e1.m128_f32[2] = 0;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);
		XMVECTOR e2 = XMEulerFromQuaternion(q1);

		SS_ASSERT(XMAlmostEqual(e1, e2));

		int a = 0;
	}

	{
		XMVECTOR v1 = {0,0,0,0};
		XMVECTOR v2 = {1,1,1,1};
		SS_ASSERT(XMAlmostEqual(v1, v2) == false);

		v1 = { 0,0,0,0 };
		v2 = { 0.1,0,0.1,0};
		SS_ASSERT(XMAlmostEqual(v1, v2) == false);

		v1 = { 0,0,0,0 };
		v2 = { 0.00001,0,0.00001,0 };
		SS_ASSERT(XMAlmostEqual(v1, v2));
	}


	{
		XMVECTOR e1;
		e1.m128_f32[0] = XM_PIDIV2;
		e1.m128_f32[1] = XM_PIDIV2;
		e1.m128_f32[2] = 0;
		e1.m128_f32[3] = 0;
		XMVECTOR Quat = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(Quat);

		SS_ASSERT(XMAlmostEqual(e1, e2));
	}


	{
		XMVECTOR e1;
		e1.m128_f32[0] = XM_PIDIV2 / 2;
		e1.m128_f32[1] = XM_PIDIV2 / 2;
		e1.m128_f32[2] = XM_PIDIV2 / 2;
		e1.m128_f32[3] = 0;
		XMVECTOR Quat = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(Quat);

		SS_ASSERT(XMAlmostEqual(e1, e2));
	}

	{
		XMVECTOR e1;
		e1.m128_f32[0] = XM_PIDIV2;
		e1.m128_f32[1] = XM_PIDIV2 / 3;
		e1.m128_f32[2] = XM_PI;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);

		SS_ASSERT(XMAlmostEqual(q1, q2));

		int a = 0;
	}

	{
		XMVECTOR e1;
		e1.m128_f32[0] = -XM_PIDIV2;
		e1.m128_f32[1] = XM_PIDIV2 / 3;
		e1.m128_f32[2] = XM_PI;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);

		SS_ASSERT(XMAlmostEqual(q1, q2));

		int a = 0;
	}

	{
		XMVECTOR e1;
		e1.m128_f32[0] = -XM_PIDIV2;
		e1.m128_f32[1] = -XM_PIDIV2 / 3;
		e1.m128_f32[2] = XM_PI;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);

		SS_ASSERT(XMAlmostEqual(q1, q2));

		int a = 0;
	}

	{
		XMVECTOR e1;
		e1.m128_f32[0] = -XM_PIDIV2;
		e1.m128_f32[1] = -XM_PIDIV2 / 3;
		e1.m128_f32[2] = -XM_PI;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);

		SS_ASSERT(XMAlmostEqual(q1, q2));

		int a = 0;
	}

	for (int i=0;i<10;i++)
	{
		XMVECTOR e1;

		srand(time(NULL));
		// generate random from -PI to PI
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[0] = r;
		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[1] = r;
		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[2] = r;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);


		constexpr float DEG_1 = 0.0174533;
		SS_ASSERT(XMAlmostEqual(q1, q2, DEG_1) || XMAlmostEqual(q1, -q2, DEG_1));
	}

	for (int i = 0; i < 10; i++)
	{
		XMVECTOR e1;

		srand(time(NULL));
		// generate random from -PI to PI
		e1.m128_f32[0] = XM_PIDIV2;
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[1] = r;
		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[2] = r;
		e1.m128_f32[3] = 0;
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);

		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);


		constexpr float DEG_1 = 0.0174533;
		SS_ASSERT(XMAlmostEqual(q1, q2, DEG_1) || XMAlmostEqual(q1, -q2, DEG_1));
	}

	for (int i=0;i<10;i++)
	{
		XMVECTOR e1;

		srand(time(NULL));
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[0] = r;
		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[1] = r;
		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (XM_2PI))) - XM_PI;
		e1.m128_f32[2] = r;
		e1.m128_f32[3] = 0;

		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(e1);
		XMVECTOR e2 = XMEulerFromQuaternion(q1);
		XMVECTOR q2 = XMQuaternionRotationRollPitchYawFromVector(e2);

		constexpr float DEG_1 = 0.0174533;
		SS_ASSERT(XMAlmostEqual(q1, q2, DEG_1) || XMAlmostEqual(q1, -q2, DEG_1));
	}
}
