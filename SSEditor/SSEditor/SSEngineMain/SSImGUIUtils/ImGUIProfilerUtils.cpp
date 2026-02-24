#include "pch.h"
#include "ImGUIProfilerUtils.h"

#include "SSEngineMain/ImGUI_Profiler.h"


int32 ProfileResultItemToU16(
	utf16* OutStr,
	int64 OutStrLen,
	const ProfileResultItem& Item,
	uint64 FrameDeltaTick, 
	uint64 FrameFrequency,
	uint64 ParentConsumedTick)
{
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;

	double ConsumedMS = (double)ItemConsumedTick / (double)FrameFrequency;
	double WholeConsumedRatio = (double)ItemConsumedTick / (double)FrameDeltaTick;
	double ParentRelativeConsumedRatio = (double)ItemConsumedTick / (double)ParentConsumedTick;
	WholeConsumedRatio *= 100;
	ParentRelativeConsumedRatio *= 100;

	int32 WrittenWordCnt = swprintf_s(
		OutStr,
		OutStrLen,

		L"%ls:\t"
		L"%.3lf ms\t"
		L"%.2lf(%.2lf) %%",

		Item.Name.C_Str(),
		ConsumedMS,
		ParentRelativeConsumedRatio, WholeConsumedRatio);

	return WrittenWordCnt;
}
