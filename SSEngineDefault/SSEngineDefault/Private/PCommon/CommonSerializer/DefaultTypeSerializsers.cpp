#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"



int64 AppendData(SS::PooledList<byte>& ToData, const void* From, int64 FromDataSize)
{
	const int64 OriginalSize = ToData.GetSize();
	const int64 OriginalCapacity = ToData.GetCapacity();

	if (OriginalCapacity - OriginalSize < FromDataSize)
	{
		// 가용가능한 공간이 새로 할당해야 할 데이터 사이즈보다 작으면 메모리를 2배로 할당
		ToData.Reserve(OriginalCapacity * 2);
	}

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


int64 FillMemoryFromData(
	void* Dest, int64 CopySize,
	const SS::PooledList<byte>& FromData, int64 FromOffset)
{
	int64 FromCapacity = FromData.GetSize() - FromOffset;
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

int64 AppendDataFromStrings(SS::PooledList<byte>& Data, const SS::PooledList<SS::StringW>& Strings)
{
	int32 TotalStrLen = 0;
	for (const SS::StringW& StringItem : Strings)
	{
		int32 ItemStrLen = StringItem.GetStrLen();
		TotalStrLen += (ItemStrLen + 1);
	}

	int64 TotalByteSizeToWrite = sizeof(int64) + (TotalStrLen * sizeof(utf16));
	// 전체 데이터 크기 = 데이터크기8byte + 전체스트링크기

	int64 OriginalByteSize = Data.GetSize();
	Data.Reserve(TotalByteSizeToWrite + OriginalByteSize);


	int64 ByteCursor = 0;
	ByteCursor += AppendData(Data, &TotalByteSizeToWrite, sizeof(TotalByteSizeToWrite));

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

int64 FillStringFromData(SS::PooledList<SS::StringW>& Strings, const SS::PooledList<byte>& Data, int64 Offset)
{
	Strings.Clear();

	const byte* RawData = Data.GetData() + Offset;

	int64 ReadDataSize;
	memcpy_s(&ReadDataSize, sizeof(ReadDataSize), RawData, sizeof(ReadDataSize));


	const int32 TotalStrLen = (ReadDataSize - sizeof(int64)) / sizeof(utf16); // TotalStrLen is including L'\0'
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int64));


	if (StrRawData[TotalStrLen - 1] != L'\0')
	{
		SS_ASSERT(false);
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

	SS_ASSERT(StrCursor * 2 + sizeof(int64) == ReadDataSize);
	return ReadDataSize;
}

int64 AppendDataFromHashers(SS::PooledList<byte>& Data, const SS::PooledList<SS::SHasherW>& Hashers)
{
	int32 TotalStrLen = 0;
	for (SS::SHasherW HasherItem : Hashers)
	{
		int32 ItemStrLen = HasherItem.GetStrLen();
		TotalStrLen += (ItemStrLen + 1);
	}

	int64 TotalByteSizeToWrite = sizeof(int64) + (TotalStrLen * sizeof(utf16));
	// 전체 데이터 크기 = 데이터크기8byte + 전체스트링크기

	int64 CurByteSize = Data.GetSize();
	Data.Reserve(CurByteSize + TotalByteSizeToWrite);



	int64 WrittenBytes = 0;
	WrittenBytes += AppendData(Data, &TotalByteSizeToWrite, sizeof(TotalByteSizeToWrite));

	for (SS::SHasherW HasherItem : Hashers)
	{
		const int32 ItemStrLen = HasherItem.GetStrLen();
		const int32 ItemByteLen = (ItemStrLen + 1) * sizeof(utf16);

		const utf16* ItemCStr = HasherItem.C_Str();

		if (ItemCStr != nullptr)
		{
			WrittenBytes += AppendData(Data, ItemCStr, ItemByteLen);
		}
		else
		{
			WrittenBytes += AppendData(Data, L"", ItemByteLen);
		}
	}

	SS_ASSERT(TotalByteSizeToWrite == WrittenBytes);
	return WrittenBytes;
}

int64 FillHashersFromData(SS::PooledList<SS::SHasherW>& Hashers, const SS::PooledList<byte>& Data, int64 Offset)
{
	Hashers.Clear();

	const byte* RawData = Data.GetData() + Offset;

	int64 ReadDataSize; // 해당 값은 ReadDataSize자체의 크기도 포함한다.
	memcpy_s(&ReadDataSize, sizeof(int64), RawData, sizeof(int64));


	const int32 TotalStrLen = (ReadDataSize - sizeof(int64)) / sizeof(utf16); // TotalStrLen is including L'\0'
	const utf16* StrRawData = reinterpret_cast<const utf16*>(RawData + sizeof(int64));


	if (StrRawData[TotalStrLen - 1] != L'\0')
	{
		SS_ASSERT(false);
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

	SS_ASSERT(StrCursor * 2 + sizeof(int64) == ReadDataSize);
	return ReadDataSize;
}
