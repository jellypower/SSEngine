#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"

class ImGUI_Profiler : INoncopyable
{
private:
	uint64 _FrequencyCapture = 0;
	uint64 _ConsumedTickCapture = 0;
	SS::PooledList<ProfileResultItem> _ProfileResultCapture;
	SS::StringW _ProfileResultCaptureStr;

	utf16 _u16StrWorkTable[1024] = { 0, };
	utf8 _u8StrWorkTable[1024] = { 0, };

public:
	void PerFrame();

private:
	void FrameOutline();

	void ProfileDetail();

	void ShowProfileResultCapture();
	int ShowProfilerItem_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, bool bShow);

	void CopyProfilerResultToClipboard();
	int CopyCaptureToClipboard_Recursion(int32 ProfileResultIdx, int64 ParentConsumedTick, int32 Depth);
};
