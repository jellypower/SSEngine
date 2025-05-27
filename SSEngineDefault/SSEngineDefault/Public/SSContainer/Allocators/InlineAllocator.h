#pragma once
#include <crtdbg.h>
#include <cstdlib>

#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

namespace SS {
	template<int64 DEFAULT_CAPACITY>
	class InlineAllocator
	{
	public:
		template<typename T>
		class Allocator_Impl
		{
		private:
			int64 _capacity = DEFAULT_CAPACITY;
			T* _data = nullptr;
			byte _inlineData[sizeof(T) * DEFAULT_CAPACITY] = { 0 };

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
				if (_data != nullptr)
					return _data;

				return (T*)_inlineData;
			}



			T* Alloc(const char* fileName, int32 lineNo, int64 inCapacity)
			{
				if(inCapacity < DEFAULT_CAPACITY)
				{
					_capacity = DEFAULT_CAPACITY;
					return (T*)_inlineData;
				}

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
				if (_data == nullptr)
				{
					return;
				}

				free(_data);
				_data = nullptr;
				_capacity = DEFAULT_CAPACITY;
			}

			void TakeAwayMem(InlineAllocator<DEFAULT_CAPACITY>::Allocator_Impl<T>& other)
			{
				if (_data != nullptr)
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
				other._capacity = DEFAULT_CAPACITY;
			}
		};
	};

}
