#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class EMaterialType
{
	None,

	DefaultPBR,
};

inline SS::SHasherW GetPSNameOfMtlType(EMaterialType Type)
{
	if (Type == EMaterialType::DefaultPBR)
	{
		static SS::SHasherW MatName(L"PS_DefaultPBR.sha");
		return MatName;
	}

	SS_ASSERT(false);
	return SS::SHasherW::GetEmpty();
}

struct MtlDataBase : public INoncopyable
{
	EMaterialType _Type = EMaterialType::None;
};