#include "FrameInfoProcessorBase.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/SSThread/SSThreadUtil.h"

constexpr double FRAME_LOW_LIMIT = 1 / 1000.0;

const utf16* PER_FRAME = L"PF";

const SS::PooledList<ProfileResultItem> FrameInfoProcessorBase::GetLastProfileResult() const
{
	return _LastProfileResult;
}

bool FrameInfoProcessorBase::IsProfileEnabled() const
{
	return _bIsProfileEnabled;
}

void FrameInfoProcessorBase::StartUpXXX()
{
	_ProfilingNameStack.Reserve(128);
	_ProfileInProgressResult.Reserve(128);
	_LastProfileResult.Reserve(128);

	_ProfilingNameStackAsStr = PER_FRAME;

	_perfFrequency = GetPerformanceFrequency();
	_FrameStartTick = GetPerofrmanceCounter();
}

void FrameInfoProcessorBase::PerFrameXXX()
{
	_frameCount++;

	_PrevFrameStartTick = _FrameStartTick;
	_FrameStartTick = GetPerofrmanceCounter();

	_deltaTick = _FrameStartTick - _PrevFrameStartTick;

	if (_deltaTick == 0)
	{
		_deltaTime = FRAME_LOW_LIMIT;
		SS_INTERRUPT();
	}
	else
	{
		_deltaTime = (double)_deltaTick / (double)_perfFrequency;
	}

	
	// Profiling
	{
		if (_bIsProfileEnabled)
		{
			_LastProfileResult = _ProfileInProgressResult;
			_ProfileInProgressResult.Clear();
		}

		if (_atomic_ProfileEnableReseve != 0 && _atomic_ProfileEnableReseve != 1)
		{
			SS_INTERRUPT();
		}

		if (_bIsProfileEnabled && _atomic_ProfileEnableReseve == 0)
		{
			_bIsProfileEnabled = false;
		}

		if (_bIsProfileEnabled == false && _atomic_ProfileEnableReseve == 1)
		{
			_bIsProfileEnabled = true;
		}
	}


	_elapsedTime += _deltaTime;
}

void FrameInfoProcessorBase::ProcessWindowResizeXXX(uint32 width, uint32 height)
{
	_windowSize.X = width;
	_windowSize.Y = height;
}

void FrameInfoProcessorBase::BeginMainProfile(SS::SHasherW RecordItemName)
{
	SS_ASSERT(SSThreadUtil::IsInMainThread());
	if (_bIsProfileEnabled == false)
	{
		return;
	}

	uint64 TickCnt = GetPerofrmanceCounter();


	// TODO: Lock?
	{
		_ProfilingNameStackAsStr += L"/";
		_ProfilingNameStackAsStr += RecordItemName.C_Str(); // 오버헤드
		_ProfilingNameStack.PushBack({ RecordItemName, TickCnt });
	}
}

void FrameInfoProcessorBase::EndMainProfile(SS::SHasherW RecordItemName)
{
	SS_ASSERT(SSThreadUtil::IsInMainThread());
	if (_bIsProfileEnabled == false)
	{
		return;
	}

	const ProfileNameTickCntPair& ProfStackTop = GetProfStackTop();

	if (ProfStackTop.Name != RecordItemName)
	{
		SS_INTERRUPT();
		return;
	}


	// TODO: Lock?
	{
		_ProfilingNameStack.PopBack();

		const uint64 NewTickCnt = GetPerofrmanceCounter();
		_ProfileInProgressResult.PushBack(
			{_ProfilingNameStackAsStr, ProfStackTop.TickCnt, NewTickCnt } // 오버헤드(큼)
		);

		const int32 OriginalStrLen = _ProfilingNameStackAsStr.GetStrLen();
		_ProfilingNameStackAsStr.CutOut(OriginalStrLen - RecordItemName.GetStrLen() - 1);

	}
}

void FrameInfoProcessorBase::RequestProfileEnable(bool bEnable)
{
	long TargetValue = bEnable ? 1 : 0;
	_InterlockedExchange(&_atomic_ProfileEnableReseve, TargetValue);
}

ProfileNameTickCntPair FrameInfoProcessorBase::GetProfStackTop() const
{
	int64 LastIdx = _ProfilingNameStack.GetSize() - 1;
	if (LastIdx == -1)
	{
		static const SS::SHasherW PerFrame = PER_FRAME;
		return { PerFrame, _FrameStartTick };
	}

	return _ProfilingNameStack[LastIdx];
}
