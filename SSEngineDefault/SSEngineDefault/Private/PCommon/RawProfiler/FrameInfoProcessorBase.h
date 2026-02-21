#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"


struct ProfileNameTickCntPair
{
	SS::SHasherW Name;
	uint64 TickCnt;
};



class FrameInfoProcessorBase : public IFrameInfoProcessor
{
private:
	SS::PooledList<ProfileNameTickCntPair> _ProfilingNameStack;
	SS::PooledList<ProfileResultItem> _ProfileInProgressResult;
	SS::PooledList<ProfileResultItem> _LastProfileResult;

public:
	virtual const SS::PooledList<ProfileResultItem> GetLastProfileResult() const override;

public:
	virtual void StartUpXXX() override;
	virtual void PerFrameXXX() override;
	virtual void ProcessWindowResizeXXX(uint32 width, uint32 height) override;

	virtual void BeginMainProfile(SS::SHasherW RecordItemName) override;
	virtual void EndMainProfile(SS::SHasherW RecordItemName) override;

private:
	ProfileNameTickCntPair GetLastProfile() const;
};

