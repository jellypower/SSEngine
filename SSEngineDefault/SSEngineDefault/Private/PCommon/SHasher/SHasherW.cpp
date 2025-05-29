#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SHasher/SHasherW.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/SSContainer/CityHash.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"
using namespace SS;

SHasherW SHasherW::Empty = SHasherW();

SHasherW::SHasherW()
{
	_hashX = 0;
}

SHasherW::SHasherW(const utf16* inStr)
	: _hashX(0)
{
	if (inStr == nullptr)
	{
		SS_INTERRUPT();
	}

	uint32 StrLen = wcslen(inStr);
	if(StrLen > SHASHER_STRLEN_MAX)
	{
		SS_INTERRUPT();
	}

	utf16 loweredStr[SHASHER_STRLEN_MAX + 1];
	LowerStr(inStr, loweredStr);
	uint32 HashedValue = CityHash32(reinterpret_cast<const char*>(loweredStr), StrLen * (sizeof(utf16) / sizeof(char) ));
	_hashX = g_HasherPool->FindOrAddHasherValue(loweredStr, StrLen, HashedValue);
}

SHasherW::SHasherW(const char* inStr)
{
	SS::StringW InStrW = inStr;
	new(this) SHasherW(InStrW.C_Str()); // 다른 생성자 호출
}

SHasherW::SHasherW(const SS::StringW& inStr)
{
	new(this) SHasherW(inStr.C_Str());
}

SHasherW::SHasherW(const SS::SHasherW& rhs)
{
	_hashX = rhs._hashX;
}

SHasherW& SHasherW::operator=(SHasherW rhs)
{
	_hashX = rhs._hashX;
	return *this;
}

bool SHasherW::IsEmpty() const
{
	return _hashX == 0;
}

bool SHasherW::operator==(SHasherW rhs) const
{
	return this->_hashX == rhs._hashX;
}

const utf16* SHasherW::C_Str(uint32* const OutStrLen) const
{
	const utf16* Result = g_HasherPool->FindC_Str(_hashX, OutStrLen);
	return Result;
}