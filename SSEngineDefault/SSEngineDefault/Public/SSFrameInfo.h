#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"


#include "SSVector.h"
#include "SSNativeKeywords.h"
#include "GlobalVariableSet/GlobalVariableSet.h"


class SSENGINEDEFAULT_MODULE SSFrameInfo
{
private:

	uint64 _perfFrequency = 0;

	uint64 _frameCount = 0;
	double _deltaTime = 0;
	double _elapsedTime = 0;
	uint64 _previousTick = 0;
	uint64 _currentTick = 0;

	double _FPSCheckStopWatch = 0;
	uint64 _frameCntDuringInFPSCheckterval = 0;
	uint64 _FPS = 0;

	uint64 _FPSCheckCnt = 0;
	uint64 _lastFPSCheckTick = 0;

	Vector2ui32 _screenSize;
	Vector2ui32 _windowSize;

public:
	static SSFrameInfo* Get()
	{
		return g_FrameInfo;
	}

	/**
	* return DeltaTime in ms
	*/
	static double GetDeltaTime() { return g_FrameInfo->_deltaTime; }
	static double GetFPS() { return g_FrameInfo->_FPS; }
	static double GetElapsedTime() { return g_FrameInfo->_elapsedTime; }
	static Vector2ui32 GetWindowSize() { return g_FrameInfo->_windowSize; }
	static uint64 GetFrameCnt() { return g_FrameInfo->_frameCount; }


public:
	void BeginFrameXXX();
	void PerFrameXXX();
	void ProcessWindowResizeXXX(uint32 width, uint32 height);

private:
	static uint64 GetTickCountSS();
};

