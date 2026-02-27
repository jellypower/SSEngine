#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SHasher/SHaserUtils.h"

#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/ModuleEntry/SSEngineDefaultModuleEntry.h"

SS::SHasherW MakeIndexedHasher(const utf16* InStr, int64 Idx)
{
	utf16 ItoaBuffer[21];
	_i64tow_s(Idx, ItoaBuffer, 21, 10);
	utf16 Concat[SHASHER_STRLEN_MAX];

	int32 ConcatStrIdx = 0;
	for (; ConcatStrIdx < SHASHER_STRLEN_MAX; ConcatStrIdx++)
	{
		utf16 CharItem = InStr[ConcatStrIdx];

		if (CharItem == L'\0')
		{
			break;
		}

		Concat[ConcatStrIdx] = CharItem;
	}

	if (ConcatStrIdx >= SHASHER_STRLEN_MAX)
	{
		SS_INTERRUPT();
		return SS::SHasherW();
	}

	int32 ItoaBufferIdx = 0;
	for (; ConcatStrIdx < SHASHER_STRLEN_MAX; ConcatStrIdx++)
	{
		utf16 CharItem = ItoaBuffer[ItoaBufferIdx++];
		Concat[ConcatStrIdx] = CharItem;

		if (CharItem == L'\0')
		{
			break;
		}
	}

	if (ConcatStrIdx > SHASHER_STRLEN_MAX)
	{
		SS_INTERRUPT();
		return SS::SHasherW();
	}

	return Concat;
}
