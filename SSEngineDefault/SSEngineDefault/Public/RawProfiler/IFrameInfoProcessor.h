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
	uint64 _PrevFrameStartTick = 0;
	uint64 _FrameStartTick = 0;

	double _FPSCheckStopWatch = 0;
	uint64 _frameCntDuringInFPSCheckterval = 0;
	uint64 _FPS = 0;

	uint64 _FPSCheckCnt = 0;
	uint64 _lastFPSCheckTick = 0;

	Vector2ui32 _screenSize;
	Vector2ui32 _windowSize;

public:

	/**
	* return DeltaTime in ms
	*/
	double GetDeltaTime() const { return _deltaTime; }
	uint64 GetDeltaTick() const { return _deltaTick; }
	double GetFPS() const { return _FPS; }
	double GetElapsedTime() const { return _elapsedTime; }
	Vector2ui32 GetWindowSize() const { return _windowSize; }
	uint64 GetFrameCnt() const { return _frameCount; }

public:
	virtual const SS::PooledList<ProfileResultItem> GetLastProfileResult() const = 0;
	virtual bool IsProfileEnabled() const = 0;

public:
	virtual void StartUpXXX() = 0;
	virtual void PerFrameXXX() = 0;
	virtual void ProcessWindowResizeXXX(uint32 width, uint32 height) = 0;

	virtual void BeginMainProfile(SS::SHasherW RecordName) = 0;
	virtual void EndMainProfile(SS::SHasherW RecordName) = 0;

	virtual void RequestProfileEnable(bool bEnable) = 0;
};

