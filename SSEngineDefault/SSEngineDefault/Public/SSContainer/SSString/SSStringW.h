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

		void Replace(const utf16* Replaced, const utf16* ToReplace)
		{
			const int32 OriginalStrLen = GetStrLen();
			const int32 ReplacedLen = wcslen(Replaced);
			if (ReplacedLen <= 0 || ReplacedLen > OriginalStrLen)
			{
				return;
			}


			const int32 ToReplaceLen = wcslen(ToReplace);


			int32 ReplacedHash = 0;
			for (int i = 0; i < ReplacedLen; i++)
			{
				ReplacedHash += int32(*(Replaced + i));
			}

			const utf16* CStr = _stringPool.GetData();
			int32 OriginalStrHash = 0;
			for (int i = 0; i < ReplacedLen; i++)
			{
				OriginalStrHash += int32(*(CStr + i));
			}


			int32 ReplacingStrLenDiff = ToReplaceLen - ReplacedLen;
			if (ReplacingStrLenDiff > 0)
			{
				int32 ReplaceCnt = 0;
				int32 RollingHash = OriginalStrHash;
				int32 i = 0;
				while(i < (OriginalStrLen - ReplacedLen + 1))
				{
					if (RollingHash == ReplacedHash &&
						wcsncmp(CStr + i, Replaced, ReplacedLen) == 0)
					{
						ReplaceCnt++;
						i += ReplacedLen;
						if (i > (OriginalStrLen - ReplacedLen))
						{
							break;
						}

						RollingHash = 0;
						for (int32 j = 0; j < ReplacedLen;j++) // Rolling Hash를 다시 계산한다.
						{
							RollingHash += int32(*(CStr + i + j));
						}
						continue;
					}

					RollingHash -= int32(*(CStr + i));
					RollingHash += int32(*(CStr + i + ReplacedLen));
					i++;
				}

				int32 LenIncrease = ReplacingStrLenDiff * ReplaceCnt;
				const int32 NewStrLen = LenIncrease + OriginalStrLen;
				_stringPool.Resize(NewStrLen + 1);
				CStr = _stringPool.GetData();
				memmove(_stringPool.GetData() + LenIncrease, _stringPool.GetData(), sizeof(utf16) * (OriginalStrLen + 1));

				RollingHash = OriginalStrHash;
				int32 StrCpyOffset = LenIncrease;
				i = 0;
				while (i < NewStrLen)
				{
					if (RollingHash == ReplacedHash &&
						wcsncmp(CStr + StrCpyOffset, Replaced, ReplacedLen) == 0)
					{
						for (int32 j = 0; j < ToReplaceLen; j++)
						{
							_stringPool[i + j] = ToReplace[j];
						}

						i += ToReplaceLen;
						StrCpyOffset += ReplacedLen;

						if (StrCpyOffset == i)
						{
							break;
						}

						RollingHash = 0;
						for (int32 j = StrCpyOffset; j < StrCpyOffset + ReplacedLen; j++)
						{
							RollingHash += int32(*(CStr + j));
						}
						continue;
					}

					_stringPool[i] = _stringPool[StrCpyOffset];
					
					RollingHash -= int32(*(CStr + StrCpyOffset));
					RollingHash += int32(*(CStr + StrCpyOffset + ReplacedLen));
					StrCpyOffset++;
					i++;
				}
			}
			else // 교체하려는 단어의 길이가 더 짧은경우
			{
				int32 RollingHash = OriginalStrHash;
				int32 CpyDstOffset = 0;
				int32 CpySrcOffset = 0;
				
				while (CpySrcOffset <= OriginalStrLen - ReplacedLen)
				{
					if (RollingHash == ReplacedHash &&
						wcsncmp(CStr + CpySrcOffset, Replaced, ReplacedLen) == 0)
					{
						for (int32 j = 0; j < ToReplaceLen; j++)
						{
							_stringPool[CpyDstOffset + j] = ToReplace[j];
						}

						CpyDstOffset += ToReplaceLen;
						CpySrcOffset += ReplacedLen;

						RollingHash = 0;
						for (int32 j = CpySrcOffset; j < CpySrcOffset + ReplacedLen; j++)
						{
							RollingHash += int32(*(CStr + j));
						}
						continue;
					}

					_stringPool[CpyDstOffset] = _stringPool[CpySrcOffset];

					RollingHash -= int32(*(CStr + CpySrcOffset));
					RollingHash += int32(*(CStr + CpySrcOffset + ReplacedLen));
					CpySrcOffset++;
					CpyDstOffset++;
				}

				for (int32 i = CpySrcOffset; i < OriginalStrLen; i++)
				{
					_stringPool[CpyDstOffset] = _stringPool[i];
					CpyDstOffset++;
				}

				int32 NewStrLen = CpyDstOffset;
				_stringPool.Resize(NewStrLen + 1);
				_stringPool[NewStrLen] = L'\0';
			}
		}
	};
}