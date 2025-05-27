#pragma once
#include <cstring>
#include <cwchar>

#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringContainerUtilsCollection.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SSContainer/Allocators/InlineAllocator.h"


namespace SS {
	class StringW
	{
	private:
		PooledList<utf16, SS::InlineAllocator<130>> _stringPool;

	public:
		StringW()
		{
			_stringPool.SetSizeDirectly(1);
			_stringPool[0] = L'\0';
		}

		StringW(const char* inStr)
		{
			int32 newStrLen = strlen(inStr);
			if (_stringPool.GetCapacity() < newStrLen)
			{
				_stringPool.Reserve(newStrLen * 2);
			}
			_stringPool.SetSizeDirectly(newStrLen + 1);
			int32 resultLen = CharStrToUTF16Str(inStr, newStrLen, _stringPool.GetData(), _stringPool.GetCapacity());
			SS_ASSERT(resultLen != 0);
		}


		StringW(const utf16* inStr)
		{
			int32 newStrLen = wcslen(inStr);
			if (_stringPool.GetCapacity() < newStrLen)
			{
				_stringPool.Reserve(newStrLen * 2);
			}
			_stringPool.SetSizeDirectly(newStrLen + 1);
			wcscpy_s(_stringPool.GetData(), _stringPool.GetCapacity(), inStr);
		}

		StringW(const StringW& rhs)
		{
			int32 newStrLen = rhs.GetStrLen();
			if (_stringPool.GetCapacity() < newStrLen)
			{
				_stringPool.Reserve(newStrLen * 2);
			}
			_stringPool.SetSizeDirectly(newStrLen + 1);
			wcscpy_s(_stringPool.GetData(), _stringPool.GetCapacity(), rhs.C_Str());
		}

		FORCEINLINE const utf16* C_Str() const { return _stringPool.GetData(); }
		FORCEINLINE int32 GetStrLen() const
		{
			int32 strLen = _stringPool.GetSize();
			return strLen <= 0 ? 0 : strLen - 1;
		}

		StringW& operator+=(const char* inStr)
		{
			int32 appendedStrLen = strlen(inStr);
			Append(inStr, appendedStrLen);
			return *this;
		}

		StringW& operator+=(const utf16* inStr)
		{
			int32 appendedStrLen = wcslen(inStr);
			Append(inStr, appendedStrLen);
			return *this;
		}

		StringW& operator+=(const StringW& inStr)
		{
			int32 appendedStrLen = inStr.GetStrLen();
			Append(inStr.C_Str(), appendedStrLen);
			return *this;
		}

		bool operator==(const StringW& other) const
		{
			return wcscmp(this->C_Str(), other.C_Str()) == 0;
		}

		void Append(const char* inStr, int32 inStrLen)
		{
			const int32 originalStrLen = GetStrLen();
			const int32 newStrLen = originalStrLen + inStrLen;
			const int32 newStrDataSize = newStrLen + 1;
			if (_stringPool.GetCapacity() < newStrDataSize)
			{
				_stringPool.Reserve(newStrDataSize * 2);
			}
			_stringPool.SetSizeDirectly(newStrDataSize);
			CharStrToUTF16Str(inStr, inStrLen, _stringPool.GetData() + originalStrLen, _stringPool.GetCapacity());
		}

		void Append(const utf16* inStr, int32 inStrLen)
		{
			const int32 originalStringLen = GetStrLen();
			const int32 newStrDataSize = originalStringLen + inStrLen + 1;
			if (_stringPool.GetCapacity() < newStrDataSize)
			{
				_stringPool.Reserve(newStrDataSize * 2);
			}
			_stringPool.SetSizeDirectly(newStrDataSize);
			wcsncpy(_stringPool.GetData() + originalStringLen, inStr, inStrLen + 1);
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
			_stringPool[newStrlen] = L'\0';
		}

		void Clear()
		{
			_stringPool.SetSizeDirectly(1);
			_stringPool[0] = L'\0';
		}

	};
}