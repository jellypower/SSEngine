#pragma once
#include <string.h>


#include "SSEngineDefault/SSNativeKeywords.h"
#include "SSEngineDefault/SSContainer/PooledList.h"
#include "SSEngineDefault/SSContainer/Allocators/InlineAllocator.h"

namespace SS {
	class StringA
	{
	private:
		PooledList<char, InlineAllocator<130>> _stringPool;

	public:

		StringA()
		{
			_stringPool.SetSizeDirectly(1);
			_stringPool[0] = '\0';
		}

		StringA(const char* inStr)
		{
			int32 newStrLen = strlen(inStr);
			if (_stringPool.GetCapacity() < newStrLen)
			{
				_stringPool.Reserve(newStrLen * 2);
			}
			_stringPool.SetSizeDirectly(newStrLen + 1);
			strcpy_s(_stringPool.GetData(), _stringPool.GetCapacity(), inStr);
		}

		StringA(const StringA& rhs)
		{
			int32 newStrLen = rhs.GetStrLen();
			if (_stringPool.GetCapacity() < newStrLen)
			{
				_stringPool.Reserve(newStrLen * 2);
			}
			_stringPool.SetSizeDirectly(newStrLen + 1);
			strcpy_s(_stringPool.GetData(), _stringPool.GetCapacity(), rhs.C_Str());
		}


	public:
		FORCEINLINE const char* C_Str() const { return _stringPool.GetData(); }
		FORCEINLINE int32 GetStrLen() const
		{
			int32 strLen = _stringPool.GetSize();
			return strLen <= 0 ? 0 : strLen - 1;
		}


		StringA& operator+=(const char* inStr)
		{
			int32 appendedStrLen = strlen(inStr);
			Append(inStr, appendedStrLen);
			return *this;
		}

		void Append(const char* inStr, int32 inStrLen)
		{
			const int32 originalStringLen = GetStrLen();
			const int32 newStrDataSize = originalStringLen + inStrLen + 1;
			if (_stringPool.GetCapacity() < newStrDataSize)
			{
				_stringPool.Reserve(newStrDataSize * 2);
			}
			_stringPool.SetSizeDirectly(newStrDataSize);
			strncpy(_stringPool.GetData() + originalStringLen, inStr, inStrLen);
		}

		void CutOut(int32 newStrlen)
		{
			int32 originalStrLen = GetStrLen();
			if (newStrlen >= originalStrLen)
			{
				DEBUG_BREAK();
				return;
			}
			_stringPool.SetSizeDirectly(newStrlen + 1);
			_stringPool[newStrlen] = '\0';
		}

		void Clear()
		{
			_stringPool.SetSizeDirectly(1);
			_stringPool[0] = '\0';
		}
	};
}
