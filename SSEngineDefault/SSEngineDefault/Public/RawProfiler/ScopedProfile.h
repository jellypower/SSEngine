#pragma once
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
