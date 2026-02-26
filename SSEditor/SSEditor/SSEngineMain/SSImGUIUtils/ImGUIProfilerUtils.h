#pragma once
struct ProfileResultItem;

int32 ProfileResultItemToU16(
	utf16* OutStr,
	int64 OutStrLen,
	const ProfileResultItem& Item,
	uint64 FrameDeltaTick, 
	uint64 FrameFrequency,
	uint64 ParentConsumedTick,
	bool bSimplify = false);
