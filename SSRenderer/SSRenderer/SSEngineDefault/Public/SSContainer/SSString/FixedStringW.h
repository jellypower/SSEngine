#pragma once

#include <string.h>
#include "StringUtilityFunctions.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"


namespace SS {
	template<int32 STR_LEN_MAX>
	class FixedStringW {
	private:
		utf16 _dataPool[STR_LEN_MAX];
		int32 _len;

	public:
		FixedStringW();
		FixedStringW(const char* data);
		FixedStringW(const utf16* data);
		FixedStringW(const FixedStringW& rhs);

		const utf16* C_Str() const { return _dataPool; }
		FORCEINLINE constexpr int32 GetCapacity() const { return STR_LEN_MAX; }
		FORCEINLINE const int32 GetLen() const { return _len; }

		void Assign(const utf16* data);
		void Assign(const utf16* data, int32 len);

		void Append(const utf16* data);
		void Append(const utf16* data, int32 len);

		void CutOut(int32 newStrlen);

		FixedStringW& operator+=(const utf16* data);
		FORCEINLINE operator const utf16* () const { return C_Str(); }

		void Clear();
	};

#pragma region FixedStringA implementation
	template <int32 STR_LEN_MAX>
	FixedStringW<STR_LEN_MAX>::FixedStringW()
	{
		_dataPool[0] = L'\0';
		_len = 0;
	}

	template <int32 STR_LEN_MAX>
	FixedStringW<STR_LEN_MAX>::FixedStringW(const char* data)
	{
		int32 dataLen = strlen(data);
		int32 resultLen = CharStrToUTF16Str(data, dataLen, _dataPool, GetCapacity());
		assert(resultLen != 0);
		_len = resultLen;
	}

	template <int32 STR_LEN_MAX>
	FixedStringW<STR_LEN_MAX>::FixedStringW(const utf16* data)
	{
		_len = wcslen(data);
		wcscpy(_dataPool, data);
	}

	template <int32 STR_LEN_MAX>
	FixedStringW<STR_LEN_MAX>::FixedStringW(const FixedStringW& rhs)
	{
		_len = rhs.GetLen();
		wcscpy(_dataPool, rhs.C_Str());
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::Assign(const utf16* data)
	{
		int32 len = wcslen(data);
		assert(len < STR_LEN_MAX);
		_len = len;
		wcscpy(_dataPool, data);
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::Assign(const utf16* data, int32 len)
	{
		assert(len < STR_LEN_MAX);
		_len = len;
		wcsncpy(_dataPool, data, len);
		_dataPool[len] = L'\0';
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::Append(const utf16* data)
	{
		int32 len = wcslen(data);
		assert(_len + len < STR_LEN_MAX);
		wcscpy(_dataPool + _len, data);
		_len += len;
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::Append(const utf16* data, int32 len)
	{
		assert(_len + len < STR_LEN_MAX);
		wcsncpy(_dataPool + _len, data, len);
		_len += len;
		_dataPool[_len] = L'\0';
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::CutOut(int32 newStrlen)
	{
		assert(newStrlen < _len);
		_len = newStrlen;
		_dataPool[_len] = L'\0';
	}

	template <int32 STR_LEN_MAX>
	FixedStringW<STR_LEN_MAX>& FixedStringW<STR_LEN_MAX>::operator+=(const utf16* data)
	{
		Append(data);
		return *this;
	}

	template <int32 STR_LEN_MAX>
	void FixedStringW<STR_LEN_MAX>::Clear()
	{
		_dataPool[0] = L'\0';
		_len = 0;
	}
#pragma endregion

}
