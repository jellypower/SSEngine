#pragma once
#include <crtdbg.h>
#include <cstdlib>

#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

namespace SS {
	class HeapAllocator
	{
	public:
		template<typename T>
		class Allocator_Impl
		{
		private:
			int64 _capacity = 0;
			T* _data = nullptr;

		public:
			FORCEINLINE int64 GetCapacity() const
			{
				return _capacity;
			}

			FORCEINLINE bool IsHeapDataAllocated() const
			{
				return _data != nullptr;
			}

			FORCEINLINE T* GetData() const
			{
				return _data;
			}



			T* Alloc(const char* fileName, int32 lineNo, int64 inCapacity)
			{
#ifdef _DEBUG
				_data = (T*)_malloc_dbg(inCapacity * sizeof(T), _NORMAL_BLOCK, fileName, lineNo);
#else
				_data = (T*)malloc(inCapacity * sizeof(T));
#endif

				_capacity = inCapacity;
				return _data;
			}

			void Free()
			{
				if(_data == nullptr)
				{
					return;
				}
				
				free(_data);
				_data = nullptr;
				_capacity = 0;
			}

			void TakeAwayMem(HeapAllocator::Allocator_Impl<T>& other)
			{
				if(_data != nullptr)
				{
					DEBUG_BREAK();
					return;
				}

				if(other.IsHeapDataAllocated() == false)
				{
					DEBUG_BREAK();
					return;
				}

				_data = other._data;
				_capacity = other._capacity;

				other._data = nullptr;
				other._capacity = 0;
			}
		};
	};

}
