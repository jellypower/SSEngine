#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSVector.h"
#include "SSEngineDefault/Public/SHasher/SHasherW.h"


struct ProfileResultItem
{
	SS::StringW ProfStr;
	uint64 TickStart;
	uint64 TickEnd;
};

class IFrameInfoProcessor : public INoncopyable
{
protected:
	uint64 _perfFrequency = 0;

	uint64 _frameCount = 0;
	uint64 _deltaTick = 0;
	double _deltaTime = 0;
	double _elapsedTime = 0;
	double _SmoothDeltaTime = 0;


public:

	/**
	* return DeltaTime in ms
	*/
	double GetDeltaTime() const { return _deltaTime; }
	double GetSmoothDeltaTime() const { return _SmoothDeltaTime; }
	uint64 GetDeltaTick() const { return _deltaTick; }
	double GetElapsedTime() const { return _elapsedTime; }
	uint64 GetFrameCnt() const { return _frameCount; }

public:
	virtual const SS::PooledList<ProfileResultItem> GetLastProfileResult() const = 0;
	virtual bool IsProfileEnabled() const = 0;

public:
	virtual void StartUpXXX() = 0;
	virtual void PerFrameXXX() = 0;

	virtual void BeginMainProfile(SS::SHasherW RecordName) = 0;
	virtual void EndMainProfile(SS::SHasherW RecordName) = 0;

	virtual void RequestProfileEnable(bool bEnable) = 0;
};

