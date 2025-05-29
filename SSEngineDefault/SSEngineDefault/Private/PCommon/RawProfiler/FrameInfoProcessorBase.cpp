#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/RawProfiler/FrameInfoProcessorBase.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"

constexpr double FRAME_LOW_LIMIT = 1 / 1000.0;



void FrameInfoProcessorBase::BeginFrameXXX()
{
	_perfFrequency = GetPerformanceFrequency();
	_currentTick = GetPerofrmanceCounter();
	_lastFPSCheckTick = _currentTick;
}

void FrameInfoProcessorBase::PerFrameXXX()
{
	_frameCount++;
	_frameCntDuringInFPSCheckterval++;

	_previousTick = _currentTick;
	_currentTick = GetPerofrmanceCounter();

	uint64 tickDiff = _currentTick - _previousTick;

	if (tickDiff == 0)
	{
		_deltaTime = FRAME_LOW_LIMIT;
		__debugbreak();
	}
	else
	{
		_deltaTime = (double)tickDiff / (double)_perfFrequency;
	}

	_FPSCheckStopWatch += _deltaTime;
	if (_FPSCheckStopWatch > 1)
	{
		_FPS = _frameCntDuringInFPSCheckterval;
		_FPSCheckStopWatch = 0;
		_frameCntDuringInFPSCheckterval = 0;
	}


	_elapsedTime += _deltaTime;
}

void FrameInfoProcessorBase::ProcessWindowResizeXXX(uint32 width, uint32 height)
{
	_windowSize.X = width;
	_windowSize.Y = height;
}
