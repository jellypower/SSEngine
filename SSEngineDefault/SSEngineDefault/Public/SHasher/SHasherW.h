#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SSEngineInlineSettings.h"


namespace SS {
	class StringW;
}

namespace SS {

	// String Hasher -> 미리 해싱된 String값을 비교하여 스트링 비교 효율을 높혀주는 클래스
	class SSENGINEDEFAULT_MODULE SHasherW
	{
	public:
		static SHasherW Empty;


	private:
		union {
			struct {
				uint32 _HashedValue; // 해쉬 상위 32비트
				uint32 _CurNodeCnt; // 해쉬 하위 32비트
			};
			uint64 _hashX; // 해쉬 64비트 전체값
		};

	public:
		SHasherW();
		SHasherW(const utf16* str);
		SHasherW(const char* inStr);
		explicit SHasherW(const SS::StringW& inStr);

		SHasherW(const SS::SHasherW& rhs);
		SHasherW& operator=(SHasherW rhs);

		bool operator==(SHasherW rhs) const;

		bool IsEmpty() const;
		const utf16* C_Str(uint32* const OutStrLen = nullptr) const;
		uint64 GetDirectValue() const { return _hashX; }

	};
};