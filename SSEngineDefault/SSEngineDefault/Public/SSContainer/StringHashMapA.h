#pragma once
#include <new>
#include "CityHash.h"
#include "SSEngineDefault/SSNativeKeywords.h"
#include "SSString/SSStringContainerUtilsCollection.h"

constexpr uint32 COLLISION_LIMIT_DEFAULT = 10;

constexpr uint32 RANDOM_PRIMENO_FOR_HASH = 12312332131244124149ULL;

enum class FindResult {
	Success,
	ResultNotFound,
	InvalidStrKey
};

enum class EraseResult {
	Success,
	ResultNotFound,
	InvalidStrKey
};

enum class InsertResult {
	Success,
	CollisionLimit,
	KeyAlreadyExist,
	InvalidStrKey
};



namespace SS {

	template <typename T, uint32 STR_LEN_MAX = PATH_LEN_MAX>
	class StringHashMapA
	{
	private:
		FixedStringA<STR_LEN_MAX>* _strPool = nullptr;
		T* _dataPool = nullptr;

		uint32 _poolCapacity;
		uint32 _collisionLimit;
		uint64 _seed;


	public:
		StringHashMapA(uint32 poolCapacity, uint32 collisionLimit = COLLISION_LIMIT_DEFAULT, uint64 seed = RANDOM_PRIMENO_FOR_HASH);
		~StringHashMapA();

		InsertResult TryInsert(const char* hashString, const T& item);
		InsertResult TryInsert(const char* hashString, const T&& item);

		FindResult TryFindIndex(const char* hashString, uint32& outIdx) const;
		FindResult TryFind(const char* hashString, T& outItem) const;

		EraseResult TryErase(const char* hashString);
		EraseResult TryErase(const char* hashString, T& outItem);

		FORCEINLINE uint32 GetCapacity() const { return _poolCapacity; }
		FORCEINLINE uint32 GetCollisionLimit() const { return _collisionLimit; }
		FORCEINLINE uint64 GetSeed() const { return _seed; }

		bool TryRebuild(uint32 newPoolCapacity, uint32 newCollisionLimit, uint64 Seed);

		T& GetDirectlyAt(const uint32 idx);
	};


#pragma region Implementation
	template<typename T, uint32 STR_LEN_MAX>
	StringHashMapA<T, STR_LEN_MAX>::StringHashMapA(uint32 poolCapacity, uint32 collisionLimit, uint64 seed)
		: _poolCapacity(poolCapacity), _collisionLimit(collisionLimit), _seed(seed)
	{
		_strPool = DBG_NEW FixedStringA<STR_LEN_MAX>[poolCapacity];
		_dataPool = (T*)DBG_MALLOC(sizeof(T) * poolCapacity);
	}


	template<typename T, uint32 STR_LEN_MAX>
	StringHashMapA<T, STR_LEN_MAX>::~StringHashMapA()
	{
		for (uint32 i = 0; i < _poolCapacity; i++)
		{
			if (_strPool[i].GetLen() != 0) _dataPool[i].~T();
		}
		free(_dataPool);
		delete _strPool;
	}

	template<typename T, uint32 STR_LEN_MAX>
	InsertResult StringHashMapA<T, STR_LEN_MAX>::TryInsert(const char* hashString, const T& item)
	{
		uint64 hashStringLen = strlen(hashString);

		if (hashStringLen <= 0 || hashStringLen >= STR_LEN_MAX) {
			assert(false);
			return InsertResult::InvalidStrKey;
		}

		uint64 hashValue = CityHash64WithSeed(hashString, hashStringLen, _seed) % _poolCapacity;
		uint64 count = 0;

		while (count < _collisionLimit) {
			if (_strPool[hashValue].GetLen() == 0) break;
			if (strcmp(_strPool[hashValue].C_Str(), hashString) == 0) return InsertResult::KeyAlreadyExist;

			count++;
			hashValue = (hashValue + 1) % _poolCapacity;
		}

		if (count >= _collisionLimit)
		{
			return InsertResult::CollisionLimit;
		}

		_strPool[hashValue].Assign(hashString);
		new(_dataPool + hashValue) T(item);
		return InsertResult::Success;
	}

	template<typename T, uint32 STR_LEN_MAX>
	InsertResult StringHashMapA<T, STR_LEN_MAX>::TryInsert(const char* hashString, const T&& item)
	{
		uint64 hashStringLen = strlen(hashString);

		if (hashStringLen <= 0 || hashStringLen >= STR_LEN_MAX) {
			assert(false);
			return InsertResult::InvalidStrKey;
		}

		uint64 hashValue = CityHash64WithSeed(hashString, hashStringLen, _seed) % _poolCapacity;
		uint64 count = 0;

		while (count < _collisionLimit) {
			if (_strPool[hashValue].GetLen() == 0) break;
			if (strcmp(_strPool[hashValue].C_Str(), hashString) == 0) return InsertResult::KeyAlreadyExist;

			count++;
			hashValue = (hashValue + 1) % _poolCapacity;
		}

		if (count >= _collisionLimit)
		{
			return InsertResult::CollisionLimit;
		}

		_strPool[hashValue].Assign(hashString);
		new(_dataPool + hashValue) T(item);
		return InsertResult::Success;
	}

	template<typename T, uint32 STR_LEN_MAX>
	FindResult StringHashMapA<T, STR_LEN_MAX>::TryFindIndex(const char* hashString, uint32& outIdx) const
	{
		uint64 hashStringLen = strlen(hashString);

		if (hashStringLen <= 0 || hashStringLen >= STR_LEN_MAX) {
			return FindResult::InvalidStrKey;
		}

		uint64 hashValue = CityHash64WithSeed(hashString, hashStringLen, _seed) % _poolCapacity;
		uint64 count = 0;

		while (count < _collisionLimit)
		{
			if (_strPool[hashValue].GetLen() != 0 && strcmp(_strPool[hashValue].C_Str(), hashString) == 0) {
				outIdx = hashValue;
				return FindResult::Success;
			}

			count++;
			hashValue = (hashValue + 1) % _poolCapacity;
		}

		outIdx = SS_UINT32_MAX;
		return FindResult::ResultNotFound;
	}
	template<typename T, uint32 STR_LEN_MAX>
	FindResult StringHashMapA<T, STR_LEN_MAX>::TryFind(const char* hashString, T& outItem) const
	{
		uint32 outIdx;
		FindResult indexFindResult = TryFindIndex(hashString, outIdx);

		if (indexFindResult != FindResult::Success) {
			return indexFindResult;
		}

		outItem = _dataPool[outIdx];
		return FindResult::Success;
	}

	template<typename T, uint32 STR_LEN_MAX>
	EraseResult StringHashMapA<T, STR_LEN_MAX>::TryErase(const char* hashString)
	{
		uint32 outIdx;
		FindResult indexFindResult = TryFindIndex(hashString, outIdx);

		switch (indexFindResult) {
		case FindResult::InvalidStrKey:
			return EraseResult::InvalidStrKey;
		case FindResult::ResultNotFound:
			return EraseResult::ResultNotFound;

		case FindResult::Success:
			_strPool[outIdx].Clear();
			return EraseResult::Success;
		}

		assert(false);
	}
	template<typename T, uint32 STR_LEN_MAX>
	EraseResult StringHashMapA<T, STR_LEN_MAX>::TryErase(const char* hashString, T& outItem)
	{
		uint32 outIdx;
		FindResult indexFindResult = TryFindIndex(hashString, outIdx);

		switch (indexFindResult) {
		case FindResult::InvalidStrKey:
			return EraseResult::InvalidStrKey;
		case FindResult::ResultNotFound:
			return EraseResult::ResultNotFound;
		case FindResult::Success:
			_strPool[outIdx].Clear();
			outItem = _dataPool[outIdx];
			_dataPool[outIdx], ~T();
			return EraseResult::Success;
		}

		assert(false);
	}

	template<typename T, uint32 STR_LEN_MAX>
	bool StringHashMapA<T, STR_LEN_MAX>::TryRebuild(uint32 newPoolCapacity, uint32 newCollisionLimit, uint64 newSeed)
	{

		FixedStringA<STR_LEN_MAX>* newStrPool = DBG_NEW FixedStringA<STR_LEN_MAX>[newPoolCapacity];
		T* newDataPool = (T*)DBG_MALLOC(sizeof(T) * newPoolCapacity);

		for (uint32 i = 0; i < _poolCapacity; i++) {

			if (_strPool[i].GetLen() != 0) {
	
				const char* hashString = _strPool[i].C_Str();
				uint32 hashStringLen = _strPool[i].GetLen();				

				uint64 hashValue = CityHash64WithSeed(hashString, hashStringLen, newSeed) % newPoolCapacity;
				uint64 count = 0;

				while (count < newCollisionLimit && newStrPool[hashValue].GetLen() != 0) {
					count++;
					hashValue = (hashValue + 1) % newPoolCapacity;
				}

				if (count >= newCollisionLimit)
				{
					return false;
				}

				newStrPool[hashValue].Assign(hashString);
				memcpy(newDataPool + hashValue, _dataPool + i, sizeof(T));
			}
		}

		_poolCapacity = newPoolCapacity;
		_collisionLimit = newCollisionLimit;
		_seed = newSeed;

		delete _strPool;
		free(_dataPool);

		_strPool = newStrPool;
		_dataPool = newDataPool;
		
		return true;
	}

	template<typename T, uint32 STR_LEN_MAX>
	T& StringHashMapA<T, STR_LEN_MAX>::GetDirectlyAt(const uint32 idx)
	{
		assert(_strPool[idx].GetLen() != 0);
		return _dataPool[idx];
	}
#pragma endregion
};