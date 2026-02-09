#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"



int32 AppendData(SS::PooledList<byte>& ToData, const void* From, int FromDataSize)
{
	const int32 OriginalSize = ToData.GetSize();
	ToData.SetSizeDirectly(OriginalSize + FromDataSize);

	byte* CopyTargetRaw = ToData.GetData() + OriginalSize;

	errno_t Result = memcpy_s(
		CopyTargetRaw, FromDataSize,
		From, FromDataSize);

	if (Result != 0)
	{
		SS_ASSERT(false);
		return 0;
	}

	return FromDataSize;
}


int32 FillMemoryFromData(
	void* Dest, int CopySize,
	const SS::PooledList<byte>& FromData, int FromOffset)
{
	int32 FromCapacity = FromData.GetSize() - FromOffset;
	if (CopySize > FromCapacity)
	{
		SS_ASSERT(false);
		return 0;
	}

	errno_t Result = memcpy_s(
		Dest, CopySize,
		FromData.GetData() + FromOffset, CopySize);

	if (Result != 0)
	{
		SS_ASSERT(false);
		return 0;
	}

	return CopySize;
}

int32 AppendDataFromStrings(SS::PooledList<byte>& Data, const SS::PooledList<SS::StringW>& Strings)
{
	int32 TotalStrLen = 0;
	for (const SS::StringW& StringItem : Strings)
	{
		int32 ItemStrLen = StringItem.GetStrLen();
		TotalStrLen += (ItemStrLen + 1);
	}

	int32 TotalByteSizeToWrite = sizeof(int32) + (TotalStrLen * sizeof(utf16));
	// 전체 데이터 크기 = 데이터크기4byte + 전체스트링크기

	int OriginalByteSize = Data.GetSize();
	Data.Reserve(TotalByteSizeToWrite + OriginalByteSize);


	int32 ByteCursor = 0;
	ByteCursor += AppendData(Data, &TotalByteSizeToWrite, sizeof(int32));

	for (const SS::StringW& StringItem : Strings)
	{
		int32 ItemStrLen = StringItem.GetStrLen();
		int32 ItemByteLen = (ItemStrLen + 1) * sizeof(utf16);

		const utf16* ItemCStr = StringItem.C_Str();

		ByteCursor += AppendData(Data, ItemCStr, ItemByteLen);
	}

	SS_ASSERT(TotalByteSizeToWrite == ByteCursor);
	return ByteCursor;
}

int32 FillStringFromData(SS::PooledList<SS::StringW>& Strings, const SS::PooledList<byte>& Data, int Offset)
{
	Strings.Clear();

	const byte* RawData = Data.GetData() + Offset;

	int32 ReadDataSize;
	memcpy_s(&ReadDataSize, sizeof(int32), RawData, sizeof(int32));


	const int32 TotalStrLen = (ReadDataSize - sizeof(int32)) / sizeof(utf16); // TotalStrLen is including L'\0'
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int32));


	if (StrRawData[TotalStrLen - 1] != L'\0')
	{
		return 0;
	}

	int32 StrCursor = 0;
	while (StrCursor < TotalStrLen)
	{
		Strings.PushBack(StrRawData + StrCursor);

		int32 CurStrCnt = Strings.GetSize();
		int32 PushedStrLen = Strings[CurStrCnt - 1].GetStrLen();

		StrCursor += (PushedStrLen + 1);
	}

	SS_ASSERT(StrCursor * 2 + sizeof(int32) == ReadDataSize);
	return ReadDataSize;
}

int32 AppendDataFromHashers(SS::PooledList<byte>& Data, const SS::PooledList<SS::SHasherW>& Hashers)
{
	int32 TotalStrLen = 0;
	for (SS::SHasherW HasherItem : Hashers)
	{
		int32 ItemStrLen = HasherItem.GetStrLen();
		TotalStrLen += (ItemStrLen + 1);
	}

	int32 TotalByteSizeToWrite = sizeof(int32) + (TotalStrLen * sizeof(utf16));
	// 전체 데이터 크기 = 데이터크기4byte + 전체스트링크기

	int CurByteSize = Data.GetSize();
	Data.Reserve(CurByteSize + TotalByteSizeToWrite);



	int32 ByteCursor = 0;
	ByteCursor += AppendData(Data, &TotalByteSizeToWrite, sizeof(int32));

	for (SS::SHasherW HasherItem : Hashers)
	{
		int32 ItemStrLen = HasherItem.GetStrLen();
		int32 ItemByteLen = (ItemStrLen + 1) * sizeof(utf16);

		const utf16* ItemCStr = HasherItem.C_Str();

		ByteCursor += AppendData(Data, ItemCStr, ItemByteLen);
	}

	SS_ASSERT(TotalByteSizeToWrite == ByteCursor);
	return ByteCursor;
}

int32 FillHashersFromData(SS::PooledList<SS::SHasherW>& Hashers, const SS::PooledList<byte>& Data, int Offset)
{
	Hashers.Clear();

	const byte* RawData = Data.GetData() + Offset;

	int32 ReadDataSize;
	memcpy_s(&ReadDataSize, sizeof(int32), RawData, sizeof(int32));


	const int32 TotalStrLen = (ReadDataSize - sizeof(int32)) / sizeof(utf16); // TotalStrLen is including L'\0'
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int32));


	if (StrRawData[TotalStrLen - 1] != L'\0')
	{
		return 0;
	}

	int32 StrCursor = 0;
	while (StrCursor < TotalStrLen)
	{
		Hashers.PushBack(StrRawData + StrCursor);

		int32 CurStrCnt = Hashers.GetSize();
		int32 PushedStrLen = Hashers[CurStrCnt - 1].GetStrLen();

		StrCursor += (PushedStrLen + 1);
	}

	SS_ASSERT(StrCursor * 2 + sizeof(int32) == ReadDataSize);
	return ReadDataSize;
}
