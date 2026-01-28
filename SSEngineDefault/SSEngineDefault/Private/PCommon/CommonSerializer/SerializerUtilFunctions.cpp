#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/CommonSerializer/StringSerializer/SerializerUtilFunctions.h"

#include "SSEngineDefault/Public/CommonSerializer/StringSerializer/StringSerailizerContainer.h"

void FillDataFromStrings(StringWSerializerContainer& Container)
{
	Container.Data.Clear();


	int32 TotalStrLen = 0;
	for (const SS::StringW& StringItem : Container.Strings)
	{
		int32 ItemStrLen = StringItem.GetStrLen();
		TotalStrLen += (ItemStrLen + 1);
	}

	int32 TotalByteSize = sizeof(int32); // StringSize를 처음 4바이트에 넣어준다.
	TotalByteSize += (TotalStrLen * sizeof(utf16)); // 그리고 스트링 길이만큼 바이트 사이즈 할당한다.

	Container.Data.SetSizeDirectly(TotalByteSize);

	byte* RawData = Container.Data.GetData();
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int32));


	int32 ByteCursor = 0;
	memcpy_s(
		RawData + ByteCursor, TotalByteSize,
		&TotalByteSize, sizeof(int32));
	ByteCursor += sizeof(int32);

	for (const SS::StringW& StringItem : Container.Strings)
	{
		int32 ItemStrLen = StringItem.GetStrLen();
		int32 ItemByteLen = (ItemStrLen + 1) * sizeof(utf16);

		const utf16* ItemCStr = StringItem.C_Str();

		memcpy_s(
			RawData + ByteCursor, TotalByteSize,
			ItemCStr, ItemByteLen);

		int a = 0;

		ByteCursor += ItemByteLen;
	}


	int a = 0;
}

bool FillStringFromData(StringWSerializerContainer& Container)
{
	Container.Strings.Clear();

	const int32 ContainerDataSize = Container.Data.GetSize();
	const byte* RawData = Container.Data.GetData();

	int32 ReadDataSize;
	memcpy_s(&ReadDataSize, ContainerDataSize, RawData, sizeof(int32));

	SS_ASSERT(ReadDataSize == ContainerDataSize);

	const int32 TotalStrLen = (ContainerDataSize - sizeof(int32)) / sizeof(utf16); // TotalStrLen is including L'\0'
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int32));


	if (StrRawData[TotalStrLen - 1] != L'\0')
	{
		return false;
	}

	int32 StrCursor = 0;
	while (StrCursor < TotalStrLen)
	{
		Container.Strings.PushBack(StrRawData + StrCursor);

		int32 CurStrCnt = Container.Strings.GetSize();
		int32 PushedStrLen = Container.Strings[CurStrCnt - 1].GetStrLen();

		StrCursor += (PushedStrLen + 1);
	}

	int a = 0;
}
