#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"

class ImGUI_Profiler : public ISSNoncopyable
{
private:
	double _AvgDeltaTimeAcc = 0;
	uint64 _AvgSampleTickCnt = 0;
	double _AvgDeltaTime = 0;

private:
	uint64 _FrequencyCapture = 0;
	uint64 _ConsumedTickCapture = 0;
	SS::PooledList<ProfileResultItem> _ProfileResultCapture;
	SS::StringW _ProfileResultCaptureStr;

	utf16 _u16StrWorkTable[1024] = { 0, };
	utf8 _u8StrWorkTable[1024] = { 0, };

	bool _bIsProfileEnabled = false;
	bool _bOverFrameAutoCapture = false;

public:
	void PerFrame();

private:
	void Calc_AvgDeltaTime();

	void Show_FrameOutline();


private:
	void Show_ProfileDetail();



	void Show_ProfileResultCapture();
	int Show_ProfilerItem_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, bool bShow);

	void CopyProfilerResultToClipboard();
	int CopyCaptureToClipboard_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, int Depth);


private:
	void CaptureLastProfile();
	void OverFrameAutoCapture();

};
