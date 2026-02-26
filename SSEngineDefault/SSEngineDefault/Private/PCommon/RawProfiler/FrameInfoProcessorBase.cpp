#include "FrameInfoProcessorBase.h"

#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"
#include "SSEngineDefault/Public/SSThread/SSThreadUtil.h"

constexpr double FRAME_LOW_LIMIT = 1 / 1000.0;


const SS::PooledList<ProfileResultItem> FrameInfoProcessorBase::GetLastProfileResult() const
{
	return _LastProfileResult;
}

void FrameInfoProcessorBase::StartUpXXX()
{
	_ProfilingNameStack.Reserve(128);
	_ProfileInProgressResult.Reserve(128);
	_LastProfileResult.Reserve(128);


	_perfFrequency = GetPerformanceFrequency();
	_FrameStartTick = GetPerofrmanceCounter();
	_lastFPSCheckTick = _FrameStartTick;
}

void FrameInfoProcessorBase::PerFrameXXX()
{
	_frameCount++;
	_frameCntDuringInFPSCheckterval++;

	_PrevFrameStartTick = _FrameStartTick;
	_FrameStartTick = GetPerofrmanceCounter();

	_LastProfileResult = _ProfileInProgressResult;
	_ProfileInProgressResult.Clear();

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

void FrameInfoProcessorBase::BeginMainProfile(SS::SHasherW RecordItemName)
{
	SS_ASSERT(SSThreadUtil::IsInMainThread());

	ProfileNameTickCntPair LastProfile = GetProfStackTop();
	SS::StringW NameConcat = LastProfile.Name.C_Str();

	NameConcat += L"/";
	NameConcat += RecordItemName.C_Str();

	SS::SHasherW NewName = NameConcat.C_Str();


	// TODO: Lock?
	{
		uint64 TickCnt = GetPerofrmanceCounter();
		_ProfilingNameStack.PushBack({ NewName, TickCnt });
	}
}

void FrameInfoProcessorBase::EndMainProfile(SS::SHasherW RecordItemName)
{
	SS_ASSERT(SSThreadUtil::IsInMainThread());

	const ProfileNameTickCntPair& ProfStackTop = GetProfStackTop();

	const utf16* ProfStackTopNameRaw = ProfStackTop.Name.C_Str();
	const int32 ProfStackTopNameStrLen = ProfStackTop.Name.GetStrLen();

	const utf16* RecordItemNameRaw = RecordItemName.C_Str();
	const int32 RecordItemNameStrLen = RecordItemName.GetStrLen();

	if (ProfStackTopNameStrLen <= RecordItemNameStrLen)
	{
		SS_ASSERT(false);
		return;
	}

	int32 CmpResult = wcscmp(ProfStackTopNameRaw + ProfStackTopNameStrLen - RecordItemNameStrLen, RecordItemNameRaw);
	if (CmpResult != 0)
	{
		SS_ASSERT(false);
		return;
	}


	// TODO: Lock?
	{
		_ProfilingNameStack.PopBack();

		const uint64 NewTickCnt = GetPerofrmanceCounter();

		int32 ProfLastIdx = _ProfileInProgressResult.GetSize() - 1;
		if (ProfLastIdx >= 0)
		{
			ProfileResultItem& ProfLastItem = _ProfileInProgressResult[ProfLastIdx];

			if (ProfLastItem.Name == ProfStackTop.Name) // 갱신만 함
			{
				ProfLastItem.TickEnd = NewTickCnt;
			}
			else
			{
				_ProfileInProgressResult.PushBack(
					{ ProfStackTop.Name, ProfStackTop.TickCnt, NewTickCnt }
				);
			}
		}
		else
		{
			_ProfileInProgressResult.PushBack(
				{ ProfStackTop.Name, ProfStackTop.TickCnt, NewTickCnt }
			);
		}

	}
}

ProfileNameTickCntPair FrameInfoProcessorBase::GetProfStackTop() const
{
	int64 LastIdx = _ProfilingNameStack.GetSize() - 1;
	if (LastIdx == -1)
	{
		static const SS::SHasherW PerFrame = L"PER_FRAME";
		return { PerFrame, _FrameStartTick };
	}

	return _ProfilingNameStack[LastIdx];
}