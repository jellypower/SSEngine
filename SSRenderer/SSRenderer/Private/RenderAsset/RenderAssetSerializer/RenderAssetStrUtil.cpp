#define SSRENDERER_MODULE_EXPORT
#include "SSRenderer/Public/RenderAssetSerializer/RenderAssetStrUtil.h"

SS::SHasherW ReplaceAssestNameNameSpace(SS::SHasherW AssetName, SS::SHasherW NSFrom, SS::SHasherW NSTo)
{
	const utf16* AssetNameRaw = AssetName.C_Str();
	const int32 AssetNameLen = AssetName.GetStrLen();

	const utf16* NSFromRaw = NSFrom.C_Str();
	const int32 NSFromLen = NSFrom.GetStrLen();


	int32 Cursor = 0;
	for (; Cursor < AssetNameLen && Cursor < NSFromLen; Cursor++)
	{
		const utf16 CharA = *(AssetNameRaw + Cursor);
		const utf16 CharB = *(NSFromRaw + Cursor);

		if (CharA != CharB)
		{
			break;
		}
	}

	if (Cursor != NSFromLen)
	{
		SS_ASSERT(false);
		return SS::SHasherW();
	}

	const utf16 ThisCahr = *(AssetNameRaw + Cursor);
	if (ThisCahr != L'/')
	{
		SS_ASSERT(false);
		return SS::SHasherW();
	}
	Cursor++;

	const utf16* NSToRaw = NSTo.C_Str();
	if (NSToRaw == nullptr)
	{
		SS::SHasherW ReplacedAssetName = (AssetNameRaw + Cursor);
		return ReplacedAssetName;
	}
	else
	{
		SS::StringW StringWorkSpace = NSToRaw;
		StringWorkSpace += L"/";
		StringWorkSpace += (AssetNameRaw + Cursor);
		SS::SHasherW ReplacedAssetName = StringWorkSpace.C_Str();
		return ReplacedAssetName;
	}
}
