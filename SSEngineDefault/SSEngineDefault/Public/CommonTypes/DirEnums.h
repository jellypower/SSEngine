#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

enum class E8Dir : int32
{
	None,

	U,
	UR,
	R,
	DR,
	D,
	DL,
	L,
	UL,

	Count
};

inline const char* to_string(E8Dir e)
{
	switch (e)
	{
	case E8Dir::None: return "E8Dir::None";
	case E8Dir::U: return "E8Dir::U";
	case E8Dir::UR: return "E8Dir::UR";
	case E8Dir::R: return "E8Dir::R";
	case E8Dir::DR: return "E8Dir::DR";
	case E8Dir::D: return "E8Dir::D";
	case E8Dir::DL: return "E8Dir::DL";
	case E8Dir::L: return "E8Dir::L";
	case E8Dir::UL: return "E8Dir::UL";
	case E8Dir::Count: return "E8Dir::Count";
	default: return "E8Dir::unknown";
	}
}
