#pragma once
#include "IHasherPool.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"

#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SSContainer/CityHash.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"


namespace SS {
	class StringW;
}

namespace SS {

	// String Hasher -> 미리 해싱된 String값을 비교하여 스트링 비교 효율을 높혀주는 클래스
	class SHasherW
	{
	public:
		static SHasherW GetEmpty() { return SHasherW(); }


	private:
		union {
			struct {
				uint32 _HashedValue; // 해쉬 상위 32비트
				uint32 _CurNodeCnt; // 해쉬 하위 32비트
			};
			uint64 _hashX; // 해쉬 64비트 전체값
		};

	public:
		SHasherW()
		{
			_hashX = 0;
		}

		SHasherW(const utf16* inStr)
		{
			if (inStr == nullptr)
			{
				SS_INTERRUPT();
			}

			uint32 StrLen = wcslen(inStr);
			if (StrLen > SHASHER_STRLEN_MAX)
			{
				SS_INTERRUPT();
			}

			utf16 loweredStr[SHASHER_STRLEN_MAX + 1];
			LowerStr(inStr, loweredStr);
			uint32 HashedValue = CityHash32(reinterpret_cast<const char*>(loweredStr), StrLen * (sizeof(utf16) / sizeof(char)));
			_hashX = g_HasherPool->FindOrAddHasherValue(loweredStr, StrLen, HashedValue);
		}

		SHasherW(const char* inStr)
		{
			SS::StringW InStrW = inStr;
			new(this) SHasherW(InStrW.C_Str()); // 다른 생성자 호출
		}


		explicit SHasherW(const SS::StringW& inStr)
		{
			new(this) SHasherW(inStr.C_Str());
		}

		SHasherW(const SS::SHasherW& rhs)
		{
			_hashX = rhs._hashX;
		}

		SHasherW& operator=(SHasherW rhs)
		{
			_hashX = rhs._hashX;
			return *this;
		}





		bool operator==(SHasherW rhs) const
		{
			return this->_hashX == rhs._hashX;
		}

		bool IsEmpty() const
		{
			return _hashX == 0;
		}

		const utf16* C_Str(uint32* const OutStrLen = nullptr) const
		{
			const utf16* Result = g_HasherPool->FindC_Str(_hashX, OutStrLen);
			return Result;
		}

		uint64 GetDirectValue() const { return _hashX; }

	};
};