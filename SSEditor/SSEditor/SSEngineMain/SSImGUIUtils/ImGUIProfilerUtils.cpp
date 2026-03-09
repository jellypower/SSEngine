#include "pch.h"
#include "ImGUIProfilerUtils.h"

#include "SSEngineMain/ImGUI_Profiler.h"


int32 ProfileResultItemToU16(
	utf16* OutStr,
	int64 OutStrLen,
	const ProfileResultItem& Item,
	uint64 FrameDeltaTick,
	uint64 FrameFrequency,
	uint64 ParentConsumedTick,
	bool bSimplify)
{
	int64 ItemConsumedTick = Item.TickEnd - Item.TickStart;

	double ConsumedMS = (double)ItemConsumedTick / (double)FrameFrequency;
	double WholeConsumedRatio = (double)ItemConsumedTick / (double)FrameDeltaTick;
	double ParentRelativeConsumedRatio = (double)ItemConsumedTick / (double)ParentConsumedTick;
	ConsumedMS *= 1000;
	WholeConsumedRatio *= 100;
	ParentRelativeConsumedRatio *= 100;

	const utf16* NameCStr = Item.ProfStr.C_Str();

	if (bSimplify)
	{
		const int32 StrLen = Item.ProfStr.GetStrLen();
		int32 StrOffset = StrLen - 1;

		for (; StrOffset >= 0; StrOffset--)
		{
			if (NameCStr[StrOffset] == L'/')
			{
				break;
			}
		}

		if (StrOffset > 0)
		{
			NameCStr = NameCStr + StrOffset + 1;
		}
	}

	int32 WrittenWordCnt = swprintf_s(
		OutStr,
		OutStrLen,

		L"%ls:\t"
		L"%.1lf ms\t"
		L"%.2lf(%.2lf) %%",

		NameCStr,
		ConsumedMS,
		ParentRelativeConsumedRatio, WholeConsumedRatio);

	return WrittenWordCnt;
}
