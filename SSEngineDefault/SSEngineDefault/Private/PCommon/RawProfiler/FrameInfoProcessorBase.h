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
	static constexpr int32 DURATION_SAMPLE_MAGIC_CNT = 60;


	// Profiling
private:
	SS::PooledList<ProfileNameTickCntPair> _ProfilingNameStack;
	SS::StringW _ProfilingNameStackAsStr;

	SS::PooledList<ProfileResultItem> _ProfileInProgressResult;
	SS::PooledList<ProfileResultItem> _LastProfileResult;

	volatile long _atomic_ProfileEnableReseve = 0;
	bool _bIsProfileEnabled = false;


private: // samples
	double _DeltaTimeSample[DURATION_SAMPLE_MAGIC_CNT] = { 0, };
	double _DeltaTimeSampleSum = 0;

private: // tick
	uint64 _PrevFrameStartTick = 0;
	uint64 _FrameStartTick = 0;

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
};

