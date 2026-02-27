#pragma once
#include "IHasherPool.h"
#include "Internal/HasherPoolAccesFunc.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"

#include "SSEngineDefault/Public/SSContainer/CityHash.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"


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
		const HasherPoolNode* _StoredNode = nullptr;

	public:
		SHasherW()
		{
			_StoredNode = nullptr;
		}

		SHasherW(const utf16* inStr)
		{
			if (inStr == nullptr)
			{
				SS_INTERRUPT();
			}

			_StoredNode = FindOrAddHasherNode(inStr);
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
			_StoredNode = rhs._StoredNode;
		}

		SHasherW& operator=(SHasherW rhs)
		{
			_StoredNode = rhs._StoredNode;
			return *this;
		}





		bool operator==(SHasherW rhs) const
		{
			return this->_StoredNode == rhs._StoredNode;
		}

		bool IsEmpty() const
		{
			return _StoredNode == nullptr;
		}

		const utf16* C_Str() const
		{
			return _StoredNode == nullptr ? nullptr : _StoredNode->_str;
		}

		int32 GetStrLen() const
		{
			return _StoredNode == nullptr ? 0 : _StoredNode->_strLen;
		}

		uint64 GetDirectValue() const
		{
			return _StoredNode == nullptr ? 0 : _StoredNode->_hashX;
		}

	};
};