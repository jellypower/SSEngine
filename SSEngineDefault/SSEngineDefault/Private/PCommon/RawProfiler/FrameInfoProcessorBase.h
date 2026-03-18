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
	SS::StringW _ProfilingNameStackAsStr;

	SS::PooledList<ProfileResultItem> _ProfileInProgressResult;
	SS::PooledList<ProfileResultItem> _LastProfileResult;

public:
	virtual const SS::PooledList<ProfileResultItem> GetLastProfileResult() const override;
	bool IsProfileEnabled() const override;

public:
	virtual void StartUpXXX() override;
	virtual void PerFrameXXX() override;

	virtual void BeginMainProfile(SS::SHasherW RecordItemName) override;
	virtual void EndMainProfile(SS::SHasherW RecordItemName) override;

	virtual void RequestProfileEnable(bool bEnable) override;

private:
	ProfileNameTickCntPair GetProfStackTop() const;

private:
	volatile long _atomic_ProfileEnableReseve = 0;
	bool _bIsProfileEnabled = false;
};

