#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSUtilityContainer.h"


namespace SS {
	template<typename T, uint32 CAPACITY>
	class FixedList {

	private:
		byte _pool[sizeof(T) * CAPACITY];
		T* _data;
		uint32 _size = 0;

	public:
		FixedList();
		~FixedList();

		void PushBack(const T& newData);
		void PushBack(T&& newData);
		void Resize(uint32 newSize);
		void Clear();

		FORCEINLINE uint32 GetSize() const { return _size; }
		constexpr uint32 GetCapacity() { return CAPACITY; }
		FORCEINLINE T* GetData() { return _data; }

		FORCEINLINE T& operator[](const uint32 idx);

	public:
		class iterator {
			friend class FixedList<T, CAPACITY>;
		private:
			uint32 _idx;
			FixedList<T, CAPACITY>& _list;

		public:
			iterator& operator++() {
				_idx++;
				return *this;
			}
			iterator& operator--() {
				_idx++;
				return *this;
			}
			bool operator==(const iterator rhs) { return _idx == rhs._idx; }
			bool operator!=(const iterator rhs) { return _idx != rhs._idx; }
			T& operator*() { return _list._data[_idx]; }

		private:
			iterator(uint32 idx, FixedList<T, CAPACITY>& list) : _idx(idx), _list(list) { }
		};
		FORCEINLINE iterator const begin() { return iterator(0, *this); }
		FORCEINLINE iterator const end() { return iterator(_size, *this); }
	};


	template<typename T, uint32 CAPACITY>
	FixedList<T, CAPACITY>::FixedList()
	{
		_data = reinterpret_cast<T*>(_pool);
		_size = 0;
	}

	template<typename T, uint32 CAPACITY>
	FixedList<T, CAPACITY>::~FixedList()
	{
		for (uint32 i = 0; i < _size; i++) {
			_data[i].~T();
		}
	}

	template<typename T, uint32 CAPACITY>
	void FixedList<T, CAPACITY>::PushBack(const T& newData)
	{
		assert(_size < CAPACITY);
		new(_data + _size) T(newData);
		_size++;
	}

	template<typename T, uint32 CAPACITY>
	void FixedList<T, CAPACITY>::PushBack(T&& newData)
	{
		assert(_size < CAPACITY);
		new(_data + _size) T(SS::forward<T>(newData));
		_size++;
	}

	template<typename T, uint32 CAPACITY>
	void FixedList<T, CAPACITY>::Resize(uint32 newSize)
	{
		assert(newSize <= CAPACITY);

		for (uint32 i = _size; i < newSize; i++)
			new(_data + _size) T();

		for (uint32 i = newSize; i < _size; i++)
			_data[i].~T();

		_size = newSize;
	}

	template<typename T, uint32 CAPACITY>
	void FixedList<T, CAPACITY>::Clear()
	{
		for (uint32 i = 0; i < _size; i++) {
			_data[i].~T();
		}
		_size = 0;
	}

	template<typename T, uint32 CAPACITY>
	T& FixedList<T, CAPACITY>::operator[](const uint32 idx)
	{
		assert(idx < CAPACITY);
		return _data[idx];
	}


}