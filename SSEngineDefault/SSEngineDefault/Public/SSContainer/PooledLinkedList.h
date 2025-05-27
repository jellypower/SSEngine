#pragma once
#include <new>
#include <string.h>

#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"


namespace SS {

	constexpr int64 LLIST_INVALID_HEAD_IDX = -1;
	constexpr int64 LLIST_INVALID_TAIL_IDX = -2;

	template<typename T> class PooledLinkedList;

	template<typename T>
	class PooledLinkedListNode {
		friend class PooledLinkedList<T>;
	private:
		int64 _prev = LLIST_INVALID_HEAD_IDX;
		int64 _next = LLIST_INVALID_TAIL_IDX;
		T _data;

	public:
		PooledLinkedListNode() { }
		PooledLinkedListNode(const T& data) { _data = data; }
		PooledLinkedListNode(T&& data) { _data = data; }

	public:
		T GetDataValue() const { return _data; }
		int64 GetPrev() const { return _prev; }
		int64 GetNext() const { return _next; }
		const T& GetDataReference() const { return _data; }
		const T* GetDataPtr() const { return &_data; }

		T& GetDataReference() { return _data; }
		T* GetDataPtr() { return &_data; }

	};


	template<typename T>
	class PooledLinkedList
	{
		typedef PooledLinkedListNode<T> NodeType;

	public:
		class iterator
		{
			friend class PooledLinkedList<T>;
		private:
			const PooledLinkedList& _list;
			int64 _nodeIdx;

		public:
			bool operator==(const iterator& rhs) const { return _nodeIdx == rhs._nodeIdx; }
			bool operator!=(const iterator& rhs) const { return _nodeIdx != rhs._nodeIdx; }
			int64 GetNodeIdx() const { return _nodeIdx; }
			const PooledLinkedList* GetLinkedListPtr() const { return &_list; }

		public:
			iterator& operator++()
			{
				const NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				_nodeIdx = CurNode.GetNext();
				return *this;
			}
			iterator& operator--()
			{
				const NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				_nodeIdx = CurNode.GetPrev();
				return *this;
			}

			T& operator*()
			{
				NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				return CurNode.GetDataReference();
			}
			T* operator->()
			{
				NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				return CurNode.GetDataPtr();
			}
		private:
			explicit iterator(const PooledLinkedList& list, int64 nodeIdx) : _list(list), _nodeIdx(nodeIdx) { }
		};

		class const_iterator
		{
			friend class PooledLinkedList<T>;
		private:
			const PooledLinkedList& _list;
			int64 _nodeIdx;

		public:
			bool operator==(const const_iterator& rhs) const { return _nodeIdx == rhs._nodeIdx; }
			bool operator!=(const const_iterator& rhs) const { return _nodeIdx != rhs._nodeIdx; }
			int64 GetNodeIdx() const { return _nodeIdx; }
			const PooledLinkedList* GetLinkedListPtr() const { return &_list; }

		public:
			const_iterator& operator++()
			{
				const NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				_nodeIdx = CurNode.GetNext();
				return *this;
			}
			const_iterator& operator--()
			{
				const NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				_nodeIdx = CurNode.GetPrev();
				return *this;
			}

			T& operator*() const
			{
				const NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				return CurNode.GetDataReference();
			}
			T* operator->() const
			{
				NodeType& CurNode = _list.GetNodeFromPool(_nodeIdx);
				return CurNode.GetDataPtr();
			}
		private:
			const_iterator(const PooledLinkedList& list, int64 nodeIdx) : _list(list), _nodeIdx(nodeIdx) { }
		};

	private:
		int64 _size = 0;
		int64 _capacity = 0;

		int64 _firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
		int64 _lastValidItemIdx = LLIST_INVALID_HEAD_IDX;

		int64 _usableAddressTopIdx = 0;
		int64* _usableAddressStack = nullptr;

		NodeType* _pool = nullptr;

	public:
		PooledLinkedList(int64 capacity = 0)
		{
			Reserve(capacity);
		}

		~PooledLinkedList()
		{
			Clear();
			free(_pool);
			free(_usableAddressStack);
		}

	public:
		PooledLinkedList(const PooledLinkedList& origin)
		{
			_size = origin._size;
			_capacity = origin._capacity;
			_firstValidItemIdx = origin._firstValidItemIdx;
			_lastValidItemIdx = origin._lastValidItemIdx;
			_usableAddressTopIdx = origin._usableAddressTopIdx;

			if (origin._capacity == 0)
			{
				_usableAddressStack = nullptr;
				_pool = nullptr;
			}
			else
			{
				_usableAddressStack = (int64*)DBG_MALLOC(sizeof(int64) * _capacity);
				memcpy_s(_usableAddressStack, sizeof(int64) * _capacity, origin._usableAddressStack, sizeof(int64) * _capacity);

				_pool = (NodeType*)DBG_MALLOC(sizeof(NodeType) * _capacity);
				memcpy_s(_pool, sizeof(NodeType) * _capacity, origin._pool, sizeof(NodeType) * _capacity);
			}
		}

		PooledLinkedList(PooledLinkedList&& origin)
		{
			_size = origin._size;
			_capacity = origin._capacity;
			_firstValidItemIdx = origin._firstValidItemIdx;
			_lastValidItemIdx = origin._lastValidItemIdx;
			_usableAddressTopIdx = origin._usableAddressTopIdx;
			_usableAddressStack = origin._usableAddressStack;
			_pool = origin._pool;

			origin._size = 0;
			origin._capacity = 0;
			origin._firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
			origin._lastValidItemIdx = LLIST_INVALID_HEAD_IDX;
			origin._usableAddressTopIdx = 0;
			origin._usableAddressStack = nullptr;
			origin._pool = nullptr;
		}

		PooledLinkedList& operator=(const PooledLinkedList& origin)
		{
			if (_usableAddressStack != nullptr)
			{
				if constexpr (std::is_trivially_destructible_v<T> == false)
				{
					Clear();
				}

				free(_pool);
				free(_usableAddressStack);
			}


			_size = origin._size;
			_capacity = origin._capacity;
			_firstValidItemIdx = origin._firstValidItemIdx;
			_lastValidItemIdx = origin._lastValidItemIdx;
			_usableAddressTopIdx = origin._usableAddressTopIdx;


			if (origin._capacity == 0)
			{
				_usableAddressStack = nullptr;
				_pool = nullptr;
			}
			else
			{
				_usableAddressStack = (int64*)DBG_MALLOC(sizeof(int64) * _capacity);
				memcpy_s(_usableAddressStack, sizeof(int64) * _capacity, origin._usableAddressStack, sizeof(int64) * _capacity);

				_pool = (NodeType*)DBG_MALLOC(sizeof(NodeType) * _capacity);
				memcpy_s(_pool, sizeof(NodeType) * _capacity, origin._pool, sizeof(NodeType) * _capacity);
			}

			return *this;
		}

		PooledLinkedList& operator=(PooledLinkedList&& origin)
		{
			if (_usableAddressStack != nullptr)
			{
				if constexpr (std::is_trivially_destructible_v<T> == false)
				{
					Clear();
				}

				free(_pool);
				free(_usableAddressStack);
			}

			_size = origin._size;
			_capacity = origin._capacity;
			_firstValidItemIdx = origin._firstValidItemIdx;
			_lastValidItemIdx = origin._lastValidItemIdx;
			_usableAddressTopIdx = origin._usableAddressTopIdx;
			_usableAddressStack = origin._usableAddressStack;
			_pool = origin._pool;

			origin._size = 0;
			origin._capacity = 0;
			origin._firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
			origin._lastValidItemIdx = LLIST_INVALID_HEAD_IDX;
			origin._usableAddressTopIdx = 0;
			origin._usableAddressStack = nullptr;
			origin._pool = nullptr;

			return *this;
		}

	public:
		int64 GetSize() const { return _size; }
		bool IsEmpty() const { return _size <= 0; }

		iterator begin() const { return iterator(*this, _firstValidItemIdx); }
		iterator end() const { return iterator(*this, LLIST_INVALID_TAIL_IDX); }
		iterator GetLastValidIter() const { return iterator(*this, _lastValidItemIdx); }


		T& GetLastItem() const { return GetNodeFromPool(_lastValidItemIdx).GetDataReference(); }
		T& GetFirstItem() const { return GetNodeFromPool(_firstValidItemIdx).GetDataReference(); }

		T& FindAt(int64 index) const
		{
			iterator iter = begin();
			for (int64 i = 0; i < index; i++)
				++iter;

			return (*iter);
		}

		iterator FindIteratorAt(int64 index) const
		{
			iterator iter = begin();
			for (int64 i = 0; i < index; i++)
				++iter;

			return iter;
		}


		iterator GetIterFromPool(int64 OffsetInPool) const
		{
			if (OffsetInPool < 0 || OffsetInPool >= _capacity)
			{
				SS_INTERRUPT();
			}

			return iterator(*this, OffsetInPool);
		}

		NodeType& GetNodeFromPool(int64 InNodeIdx) const
		{
			if (InNodeIdx < 0 || InNodeIdx >= _capacity)
			{
				SS_INTERRUPT();
			}


			return _pool[InNodeIdx];
		}

	public:

		void Reserve(int64 NewCapacity)
		{
			if (NewCapacity == 0)
			{
				return;
			}

			if (NewCapacity <= _capacity)
			{
				return;
			}

			int64* NewAddressStack = (int64*)DBG_MALLOC(sizeof(int64) * NewCapacity);
			if (_usableAddressStack != nullptr)
			{
				memcpy_s(NewAddressStack, _capacity * sizeof(int64), _usableAddressStack, _capacity * sizeof(int64));
				free(_usableAddressStack);
			}
			for (int64 i = _capacity; i < NewCapacity; i++)
			{
				NewAddressStack[i] = i;
			}
			_usableAddressStack = NewAddressStack;

			NodeType* NewPool = (NodeType*)DBG_MALLOC(sizeof(NodeType) * NewCapacity);
			if (_pool != nullptr)
			{
				memcpy_s(NewPool, _capacity * sizeof(NodeType), _pool, _capacity * sizeof(NodeType));
				free(_pool);
			}
			_pool = NewPool;

			_capacity = NewCapacity;
		}

		void PushBack(const T& item)
		{
			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 NewUsableAddr = AllocNewUsablePoolIdx();
			NodeType& NewTailNode = GetNodeFromPool(NewUsableAddr);
			new(&NewTailNode) NodeType(item);
			NewTailNode._next = LLIST_INVALID_TAIL_IDX;
			NewTailNode._prev = _lastValidItemIdx;


			if (_lastValidItemIdx == LLIST_INVALID_HEAD_IDX)
			{
				_firstValidItemIdx = NewUsableAddr;
				_lastValidItemIdx = NewUsableAddr;
			}
			else
			{
				NodeType& PrevTailNode = GetNodeFromPool(_lastValidItemIdx);
				PrevTailNode._next = NewUsableAddr;
				_lastValidItemIdx = NewUsableAddr;
			}

			_size++;
		}

		void PushBack(T&& item)
		{
			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 NewUsableAddr = AllocNewUsablePoolIdx();
			NodeType& NewTailNode = GetNodeFromPool(NewUsableAddr);
			new(&NewTailNode) NodeType(item);
			NewTailNode._next = LLIST_INVALID_TAIL_IDX;
			NewTailNode._prev = _lastValidItemIdx;


			if (_lastValidItemIdx == LLIST_INVALID_HEAD_IDX)
			{
				_firstValidItemIdx = NewUsableAddr;
				_lastValidItemIdx = NewUsableAddr;
			}
			else
			{
				NodeType& PrevTailNode = GetNodeFromPool(_lastValidItemIdx);
				PrevTailNode._next = NewUsableAddr;
				_lastValidItemIdx = NewUsableAddr;
			}

			_size++;
		}


		void PopBack()
		{
			if (_lastValidItemIdx == LLIST_INVALID_HEAD_IDX)
			{
				SS_INTERRUPT();
			}

			int64 NodeToDeleteIdx = _lastValidItemIdx;
			NodeType& NodeToDelete = GetNodeFromPool(_lastValidItemIdx);

			if (NodeToDelete._prev == LLIST_INVALID_HEAD_IDX)
			{
				_lastValidItemIdx = LLIST_INVALID_HEAD_IDX;
				_firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
			}
			else
			{
				NodeType& NewTailNode = GetNodeFromPool(NodeToDelete._prev);
				NewTailNode._next = LLIST_INVALID_TAIL_IDX;
				_lastValidItemIdx = NodeToDelete._prev;
			}

			NodeToDelete._prev = LLIST_INVALID_HEAD_IDX;
			NodeToDelete._next = LLIST_INVALID_TAIL_IDX;
			NodeToDelete._data.~T();

			ReleaseUsableAddress(NodeToDeleteIdx);
			_size--;
		}

		void PushFront(const T& item)
		{
			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 NewUsableAddr = AllocNewUsablePoolIdx();
			NodeType& NewHeadNode = GetNodeFromPool(NewUsableAddr);
			new(&NewHeadNode) NodeType(item);
			NewHeadNode._next = _firstValidItemIdx;
			NewHeadNode._prev = LLIST_INVALID_HEAD_IDX;

			if (_firstValidItemIdx == LLIST_INVALID_TAIL_IDX)
			{
				_lastValidItemIdx = NewUsableAddr;
				_firstValidItemIdx = NewUsableAddr;
			}
			else
			{
				NodeType& PrevHeadNode = GetNodeFromPool(_firstValidItemIdx);
				PrevHeadNode._prev = NewUsableAddr;
				_firstValidItemIdx = NewUsableAddr;
			}

			_size++;
		}

		void PushFront(T&& item)
		{
			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 NewUsableAddr = AllocNewUsablePoolIdx();
			NodeType& NewHeadNode = GetNodeFromPool(NewUsableAddr);
			new(&NewHeadNode) NodeType(item);
			NewHeadNode._next = _firstValidItemIdx;
			NewHeadNode._prev = LLIST_INVALID_HEAD_IDX;

			if (_firstValidItemIdx == LLIST_INVALID_TAIL_IDX)
			{
				_lastValidItemIdx = NewUsableAddr;
				_firstValidItemIdx = NewUsableAddr;
			}
			else
			{
				NodeType& PrevHeadNode = GetNodeFromPool(_firstValidItemIdx);
				PrevHeadNode._prev = NewUsableAddr;
				_firstValidItemIdx = NewUsableAddr;
			}

			_size++;
		}

		void PopFront()
		{
			if (_firstValidItemIdx == LLIST_INVALID_TAIL_IDX)
			{
				SS_INTERRUPT();
			}

			int64 NodeToDeleteIdx = _firstValidItemIdx;
			NodeType& NodeToDelete = GetNodeFromPool(_firstValidItemIdx);

			if (NodeToDelete._next == LLIST_INVALID_TAIL_IDX)
			{
				_firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
				_lastValidItemIdx = LLIST_INVALID_HEAD_IDX;
			}
			else
			{
				NodeType& NewHeadNode = GetNodeFromPool(NodeToDelete._next);
				NewHeadNode._prev = LLIST_INVALID_HEAD_IDX;
				_firstValidItemIdx = NodeToDelete._next;
			}

			NodeToDelete._prev = LLIST_INVALID_HEAD_IDX;
			NodeToDelete._next = LLIST_INVALID_TAIL_IDX;
			NodeToDelete._data.~T();

			ReleaseUsableAddress(NodeToDeleteIdx);
			_size--;
		}

		void InsertBack(iterator iter, const T& item)
		{
			if (iter.GetLinkedListPtr() != this)
			{
				SS_INTERRUPT();
			}

			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 TargetNodeIdx = iter.GetNodeIdx();
			NodeType& TargetNode = GetNodeFromPool(TargetNodeIdx);

			int64 NewNodeIdx = AllocNewUsablePoolIdx();
			NodeType& NewNode = GetNodeFromPool(NewNodeIdx);
			new(&NewNode) NodeType(item);
			NewNode._prev = TargetNodeIdx;
			NewNode._next = TargetNode._next;

			if (TargetNode._next == LLIST_INVALID_TAIL_IDX)
			{
				_lastValidItemIdx = NewNodeIdx;
			}
			else
			{
				NodeType& TargetNodeOriginalNext = GetNodeFromPool(TargetNode._next);
				TargetNodeOriginalNext._prev = NewNodeIdx;
			}
			TargetNode._next = NewNodeIdx;

			_size++;
		}

		void InsertBack(iterator iter, T&& item)
		{
			if (iter.GetLinkedListPtr() != this)
			{
				SS_INTERRUPT();
			}

			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 TargetNodeIdx = iter.GetNodeIdx();
			NodeType& TargetNode = GetNodeFromPool(TargetNodeIdx);

			int64 NewNodeIdx = AllocNewUsablePoolIdx();
			NodeType& NewNode = GetNodeFromPool(NewNodeIdx);
			new(&NewNode) NodeType(item);
			NewNode._prev = TargetNodeIdx;
			NewNode._next = TargetNode._next;

			if (TargetNode._next == LLIST_INVALID_TAIL_IDX)
			{
				_lastValidItemIdx = NewNodeIdx;
			}
			else
			{
				NodeType& TargetNodeOriginalNext = GetNodeFromPool(TargetNode._next);
				TargetNodeOriginalNext._prev = NewNodeIdx;
			}
			TargetNode._next = NewNodeIdx;

			_size++;
		}

		void InsertFront(iterator iter, const T& item)
		{
			if (iter.GetLinkedListPtr() != this)
			{
				SS_INTERRUPT();
			}

			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 TargetNodeIdx = iter.GetNodeIdx();
			NodeType& TargetNode = GetNodeFromPool(TargetNodeIdx);

			int64 NewNodeIdx = AllocNewUsablePoolIdx();
			NodeType& NewNode = GetNodeFromPool(NewNodeIdx);
			new(&NewNode) NodeType(item);
			NewNode._next = TargetNodeIdx;
			NewNode._prev = TargetNode._prev;

			if (TargetNode._prev == LLIST_INVALID_HEAD_IDX)
			{
				_firstValidItemIdx = NewNodeIdx;
			}
			else
			{
				NodeType& TargetNodeOriginalNext = GetNodeFromPool(TargetNode._prev);
				TargetNodeOriginalNext._next = NewNodeIdx;
			}
			TargetNode._prev = NewNodeIdx;

			_size++;
		}
		void InsertFront(iterator iter, T&& item)
		{
			if (iter.GetLinkedListPtr() != this)
			{
				SS_INTERRUPT();
			}

			if (_size >= _capacity)
			{
				Reserve((_capacity + 1) * 2);
			}

			int64 TargetNodeIdx = iter.GetNodeIdx();
			NodeType& TargetNode = GetNodeFromPool(TargetNodeIdx);

			int64 NewNodeIdx = AllocNewUsablePoolIdx();
			NodeType& NewNode = GetNodeFromPool(NewNodeIdx);
			new(&NewNode) NodeType(item);
			NewNode._next = TargetNodeIdx;
			NewNode._prev = TargetNode._prev;

			if (TargetNode._prev == LLIST_INVALID_HEAD_IDX)
			{
				_firstValidItemIdx = NewNodeIdx;
			}
			else
			{
				NodeType& TargetNodeOriginalNext = GetNodeFromPool(TargetNode._prev);
				TargetNodeOriginalNext._next = NewNodeIdx;
			}
			TargetNode._prev = NewNodeIdx;

			_size++;
		}

		iterator Erase(iterator iter) {
			int64 NodeToDeleteIdx = iter.GetNodeIdx();
			NodeType& NodeToDelete = GetNodeFromPool(NodeToDeleteIdx);


			int64 PrevNodeIdx = NodeToDelete.GetPrev();
			int64 NextNodeIdx = NodeToDelete.GetNext();

			if (PrevNodeIdx == LLIST_INVALID_HEAD_IDX && NextNodeIdx == LLIST_INVALID_TAIL_IDX)
			{
				_firstValidItemIdx = LLIST_INVALID_TAIL_IDX;
				_lastValidItemIdx = LLIST_INVALID_HEAD_IDX;
			}
			else if (NextNodeIdx == LLIST_INVALID_TAIL_IDX)
			{
				NodeType& PrevNode = GetNodeFromPool(PrevNodeIdx);
				PrevNode._next = LLIST_INVALID_TAIL_IDX;
				_lastValidItemIdx = PrevNodeIdx;
			}
			else if (PrevNodeIdx == LLIST_INVALID_HEAD_IDX)
			{
				NodeType& NextNode = GetNodeFromPool(NextNodeIdx);
				NextNode._prev = LLIST_INVALID_HEAD_IDX;
				_firstValidItemIdx = NextNodeIdx;
			}
			else
			{
				NodeType& PrevNode = GetNodeFromPool(PrevNodeIdx);
				NodeType& NextNode = GetNodeFromPool(NextNodeIdx);
				PrevNode._next = NextNodeIdx;
				NextNode._prev = PrevNodeIdx;
			}



			NodeToDelete._prev = LLIST_INVALID_HEAD_IDX;
			NodeToDelete._next = LLIST_INVALID_TAIL_IDX;
			NodeToDelete._data.~T();

			ReleaseUsableAddress(NodeToDeleteIdx);
			_size--;

			return iterator(*this, NextNodeIdx);
		}

		void Clear()
		{
			while (!IsEmpty()) {
				PopBack();
			}
		}


	private:
		int64 AllocNewUsablePoolIdx()
		{
			if (_usableAddressTopIdx >= _capacity)
			{
				SS_INTERRUPT();
			}

			int64 RetValue = _usableAddressStack[_usableAddressTopIdx];
			_usableAddressStack[_usableAddressTopIdx] = INVALID_IDX;
			_usableAddressTopIdx++;

			return RetValue;
		}

		void ReleaseUsableAddress(int64 IdxToRelease)
		{
			if (_usableAddressTopIdx < 0)
			{
				SS_INTERRUPT();
			}

			_usableAddressStack[--_usableAddressTopIdx] = IdxToRelease;
		}
	};
}