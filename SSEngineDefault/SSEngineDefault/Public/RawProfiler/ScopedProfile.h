#pragma once
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"
#include "SSEngineDefault/Public/SHasher/SHaserUtils.h"
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"


class ScopedProfile
{
private:
	SS::SHasherW _ThisProfItemName;

public:
	ScopedProfile(SS::SHasherW ProfileName)
	{
		_ThisProfItemName = ProfileName;
		g_FrameInfoProcessor->BeginMainProfile(_ThisProfItemName);
	}

	~ScopedProfile()
	{
		g_FrameInfoProcessor->EndMainProfile(_ThisProfItemName);
	}
};


class ScopedProfileIndexed
{
private:
	SS::SHasherW _ThisProfItemName;

public:
	ScopedProfileIndexed(const utf16* ProfileName, int64 Idx)
	{
		_ThisProfItemName = MakeIndexedHasher(ProfileName, Idx);
		g_FrameInfoProcessor->BeginMainProfile(_ThisProfItemName);
	}

	~ScopedProfileIndexed()
	{
		g_FrameInfoProcessor->EndMainProfile(_ThisProfItemName);
	}
};