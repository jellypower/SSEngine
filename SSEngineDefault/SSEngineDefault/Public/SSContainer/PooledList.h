#pragma once
#include <cassert>
#include <new>
#include <cstring>
#include <typeinfo>


#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSUtilityContainer.h"
#include "Allocators/HeapAllocator.h"

namespace SS
{
	template<typename T, typename AllocatorType = HeapAllocator>
	class PooledList {

	private:
		int64 _size = 0;

		typename AllocatorType::template Allocator_Impl<T> _allocator;

	public:
		PooledList(int64 capacity = 0)
		{
			_size = 0;
			if (capacity > 0) {
				_allocator.Alloc(__FILE__, __LINE__, capacity);
			}
		}

		PooledList(const PooledList& origin)
		{
			_size = origin._size;
			_allocator.Alloc(__FILE__, __LINE__, origin._size);

			for (int64 i = 0; i < _size; i++)
			{
				new(_allocator.GetData() + i) T(origin[i]);
			}
		}

		PooledList(PooledList&& origin)
		{

			if (origin._allocator.IsHeapDataAllocated() == false)
			{
				new(this) PooledList(origin);
				return;
			}

			_size = origin._size;
			_allocator.TakeAwayMem(origin._allocator);

			origin._size = 0;
		}

		PooledList(std::initializer_list<T> origin)
		{
			Reserve(origin.size());

			// UE에 따르면 initializer_list의 이터레이터가 원론적으론 일련의 메모리에 할당되지 않지만
			// 실제 구현상으론 UE가 돌아가는 모든 플랫폼에서 일련의 메모리에 할당돼있다고 한다.
			const T* originData = origin.begin();
			_size = origin.size();
			for (int64 i = 0; i < _size; i++)
			{
				new(_allocator.GetData() + i) T(originData[i]);
			}
		}

		~PooledList()
		{
			if(_allocator.GetData() != nullptr)
			{
				for (int64 i = 0; i < _size; i++)
				{
					_allocator.GetData()[i].~T();
				}
				_allocator.Free();
			}
		}

		void PushBack(const T& newData)
		{
			if (_size >= _allocator.GetCapacity())
			{
				Reserve((GetCapacity() + 1) * 2);
			} 
			::new((T*)(_allocator.GetData() + _size)) T(newData);
			_size++;
		}

		void PushBack(T&& newData)
		{
			if (_size >= _allocator.GetCapacity())
			{
				Reserve((GetCapacity() + 1) * 2);
			}
			::new((T*)(_allocator.GetData() + _size)) T(SS::forward<T>(newData));
			_size++;
		}

		void RemoveAt(int64 Idx)
		{
			if (Idx >= _size)
			{
				SS_INTERRUPT();
				return;
			}

			T* dataToRemovePtr = _allocator.GetData() + Idx;
			dataToRemovePtr->~T();
			int64 sizeToMove = _size - Idx - 1;
			memmove(dataToRemovePtr, dataToRemovePtr + 1, sizeToMove);
			_size--;
		}

		void RemoveAtAndFillLast(int64 Idx)
		{
			if (Idx >= _size)
			{
				SS_INTERRUPT();
				return;
			}

			T* dataToRemove = _allocator.GetData() + Idx;
			dataToRemove->~T();

			if (_size == Idx + 1)
			{
				_size--;
				return;
			}

			T* lastData = _allocator.GetData() + (_size - 1);
			memcpy(dataToRemove, lastData, sizeof(T));
			_size--;
		}


		void Resize(int64 newSize)
		{
			if (newSize > _allocator.GetCapacity())
			{
				Reserve(newSize);
			}


			if  constexpr (std::is_trivially_constructible_v<T> == false)
			{
				for (int64 i = _size; i < newSize; i++)
					new(_allocator.GetData() + i) T();
			}


			if  constexpr (std::is_trivially_destructible_v<T> == false)
			{
				for (int64 i = newSize; i < _size; i++)
					_allocator.GetData()[i].~T();
			}

			_size = newSize;
		}

		void SetSizeDirectly(int64 newSize)
		{
			if(newSize > _allocator.GetCapacity())
			{
				Reserve(newSize);
			}

			_size = newSize;
		}

		void Clear()
		{
			for (int64 i = 0; i < _size; i++) {
				_allocator.GetData()[i].~T();
			}
			_size = 0;
		}


		void Reserve(int64 newCapacity)
		{
			if (newCapacity <= _allocator.GetCapacity())
			{
				return;
			}

			bool IsPrevDataHeapData = _allocator.IsHeapDataAllocated();
			T* prevData = _allocator.GetData();
			T* newData = _allocator.Alloc(typeid(T).name(), __LINE__, newCapacity);
			if(prevData == newData)
			{
				return;
			}

			memcpy(newData, prevData, sizeof(T) * _size);
			if(IsPrevDataHeapData)
			{
				free(prevData);
			}
		}


		FORCEINLINE bool IsFull() const { return _size == _allocator.GetCapacity(); }
		bool IsValidIndex(int64 idx) const
		{
			return 0 <= idx && idx < _size;
		}

		FORCEINLINE int64 GetSize() const { return _size; }
		FORCEINLINE int64 GetCapacity() const { return _allocator.GetCapacity(); }
		FORCEINLINE T* GetData() const { return _allocator.GetData(); }

		FORCEINLINE const T& operator[](const int64 idx)  const
		{
			if (IsValidIndex(idx) == false)
			{
				SS_INTERRUPT();
			}
			return _allocator.GetData()[idx];
		}

		FORCEINLINE T& operator[](const int64 idx)
		{
			if (IsValidIndex(idx) == false)
			{
				SS_INTERRUPT();
			}
			return _allocator.GetData()[idx];
		}

		PooledList& operator=(PooledList&& origin)
		{
			if (origin._allocator.IsHeapDataAllocated() == false)
			{
				operator=(origin);
				return *this;
			}

			if (_allocator.GetData() != nullptr)
			{
				for (int64 i = 0; i < _size; i++)
				{
					_allocator.GetData()[i].~T();
				}
				_allocator.Free();
			}

			_size = origin._size;
			_allocator.TakeAwayMem(origin._allocator);

			origin._size = 0;
			return *this;
		}

		PooledList& operator=(const PooledList& origin)
		{
			if(_allocator.GetData() != nullptr)
			{
				for (int64 i = 0; i < _size; i++)
				{
					_allocator.GetData()[i].~T();
				}
				_allocator.Free();
			}

			_size = origin._size;
			_allocator.Alloc(__FILE__, __LINE__, origin._size);

			for (int64 i = 0; i < _size; i++)
			{
				new(_allocator.GetData() + i) T(origin[i]);
			}

			return *this;
		}

	public:
		class iterator {
			friend class PooledList;
		private:
			int64 _idx;
			const PooledList& _list;

		public:
			iterator& operator++() {
				_idx++;
				return *this;
			}
			iterator& operator--() {
				_idx--;
				return *this;
			}
			bool operator==(const iterator rhs) const { return _idx == rhs._idx; }
			bool operator!=(const iterator rhs) const { return _idx != rhs._idx; }
			T& operator*() { return _list._allocator.GetData()[_idx]; }

		private:
			iterator(int64 idx, PooledList& list) : _idx(idx), _list(list) { }
		};
		FORCEINLINE iterator begin() { return iterator(0, *this); }
		FORCEINLINE iterator end() { return iterator(_size, *this); }


		class const_iterator {
			friend class PooledList;
		private:
			int64 _idx;
			const PooledList* _list;

		public:
			const_iterator& operator++() {
				_idx++;
				return *this;
			}
			const_iterator& operator--() {
				_idx--;
				return *this;
			}
			bool operator==(const const_iterator rhs) const { return _idx == rhs._idx; }
			bool operator!=(const const_iterator rhs) const { return _idx != rhs._idx; }
			const T& operator*() const { return _list->_allocator.GetData()[_idx]; }

		private:
			const_iterator(int64 idx, const PooledList* list) : _idx(idx), _list(list) { }
		};
		FORCEINLINE const_iterator begin() const { return const_iterator(0, this); }
		FORCEINLINE const_iterator end() const { return const_iterator(_size, this); }
	};
}
