#include "SSEngineDefault/Public/SSContainer/SHasherW.h"

#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SSContainer/CityHash.h"
#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"
using namespace SS;


SS::SHashPoolNode::SHashPoolNode()
{
}

SHashPoolNode::SHashPoolNode(const utf16* inStr, uint32 inStrLen)
{
	_strLen = inStrLen;
	_str = DBG_NEW utf16[_strLen + 1];
	memcpy(_str, inStr, (inStrLen + 1) * sizeof(utf16));
}


SHasherW SHasherW::GetEmpty()
{
	static SHasherW Empty(HASHER_EMPTY);
	return Empty;
}

SHasherW::SHasherW()
{
	*this = SHasherW::GetEmpty();
}

SHasherW::SHasherW(const char* inStr)
{
	SS::StringW InStrW = inStr;
	new(this) SHasherW(InStrW.C_Str()); // 다른 생성자 호출
}

SHasherW::SHasherW(const utf16* inStr)
	: _hashX(0)
{
	const int64 inStrlen = wcslen(inStr);
	if(inStrlen > SHASHER_STRLEN_MAX)
	{
		SS_INTERRUPT();
		_hashX = SHasherW::GetEmpty()._hashX;
		return;
	}

	utf16 loweredStr[SHASHER_STRLEN_MAX + 1];

	
	LowerStr(inStr, loweredStr);


	const uint32 strHashValue = CityHash32(reinterpret_cast<const char*>(loweredStr), inStrlen * (sizeof(utf16) / sizeof(char) )) % g_sHasherPoolCnt;

	SHashPoolNode* curHashPoolNode = &g_SHasherPool[strHashValue];
	uint32 sameHashValueCnt = 0;

	if (curHashPoolNode->_str == nullptr) // 처음에 해당 해쉬 어레이가 메모리 풀만 할당되고 아무 값도 들어있지 않은 경우
	{
		new(curHashPoolNode) SHashPoolNode(loweredStr, inStrlen);
		_hashH = strHashValue;
		_hashL = sameHashValueCnt;
		return;
	}

	while (true)
	{
		if (wcscmp(curHashPoolNode->_str, loweredStr) == 0)
		{
			_hashH = strHashValue;
			_hashL = sameHashValueCnt;
			return;
		}

		if (curHashPoolNode->_next == nullptr)
		{
			break;
		}
		else
		{
			sameHashValueCnt++;
			curHashPoolNode = curHashPoolNode->_next;
		}
	}

	sameHashValueCnt++;
	curHashPoolNode->_next = DBG_NEW SHashPoolNode(loweredStr, inStrlen);

	_hashH = strHashValue;
	_hashL = sameHashValueCnt;
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
	return this->GetDirectValue() == GetEmpty().GetDirectValue();
}

bool SHasherW::operator==(SHasherW rhs) const
{
	return this->_hashX == rhs._hashX;
}

const utf16* SHasherW::C_Str(uint32* const outStrLen) const
{
	SHashPoolNode* curHashPoolNode = &g_SHasherPool[_hashH];

	for (uint32 i = 0; i < _hashL; i++)
	{
		curHashPoolNode = curHashPoolNode->_next;
	}

	if (outStrLen != nullptr)
	{
		*outStrLen = curHashPoolNode->_strLen;
	}

	return curHashPoolNode->_str;
}