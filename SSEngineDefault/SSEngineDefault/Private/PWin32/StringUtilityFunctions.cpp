#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SSContainer/SSString/StringUtilityFunctions.h"

#include <locale>
#include <Windows.h>

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"

int32 UTF16StrLenOfCharStr(const char* charStr, int32 charLen)
{
	if(charLen == INVALID_IDX)
	{
		charLen = strlen(charStr);
		int32 convertedStrLen = MultiByteToWideChar(CP_ACP, 0, charStr, charLen, nullptr, 0);
		return convertedStrLen;
	}
	else
	{
		int32 convertedStrLen = MultiByteToWideChar(CP_ACP, 0, charStr, charLen, nullptr, 0);
		return convertedStrLen;
	}
}

int32 CharStrToUTF16Str(const char* charStr, int32 charLen, utf16* outUtf16Str, int32 utf16StrBufferSize)
{
	int32 multibyteLen = MultiByteToWideChar(CP_ACP, 0, charStr, charLen, nullptr, 0);
	SS_ASSERT(multibyteLen < utf16StrBufferSize);
	multibyteLen = MultiByteToWideChar(CP_ACP, 0, charStr, charLen, outUtf16Str, utf16StrBufferSize);
	outUtf16Str[multibyteLen] = L'\0';
	return multibyteLen;
}

int32 UTF8StrToUTF16Str(const char* charStr, int32 charLen, utf16* outUtf16Str, int32 utf16StrBufferSize)
{
	int32 multibyteLen = MultiByteToWideChar(CP_UTF8, 0, charStr, charLen, nullptr, 0);
	SS_ASSERT(multibyteLen < utf16StrBufferSize);
	multibyteLen = MultiByteToWideChar(CP_UTF8, 0, charStr, charLen, outUtf16Str, utf16StrBufferSize);
	outUtf16Str[multibyteLen] = L'\0';
	return multibyteLen;
}

int32 UTF16StrToCharStr(const utf16* utf16Str, int32 utf16StrLen, char* outCharStr, int32 mbBufferSize)
{
	int32 writtenBytes = WideCharToMultiByte(CP_ACP, 0, utf16Str, utf16StrLen, NULL, 0, NULL, NULL);
	SS_ASSERT(writtenBytes < mbBufferSize);
	writtenBytes = WideCharToMultiByte(CP_ACP, 0, utf16Str, utf16StrLen, outCharStr, writtenBytes, NULL, NULL);
	outCharStr[writtenBytes] = '\0';
	return writtenBytes;
}

int32 UTF16StrToUtf8Str(const utf16* utf16Str, int32 utf16StrLen, utf8* outUtf8Str, int32 utf8BufferSize)
{
	int32 writtenBytes = WideCharToMultiByte(CP_UTF8, 0, utf16Str, utf16StrLen, NULL, 0, NULL, NULL);
	SS_ASSERT(writtenBytes < utf8BufferSize);
	writtenBytes = WideCharToMultiByte(CP_UTF8, 0, utf16Str, utf16StrLen, outUtf8Str, writtenBytes, NULL, NULL);
	outUtf8Str[writtenBytes] = '\0';
	return writtenBytes;
}

SS::StringW IntToString(int32 num)
{
	wchar_t buffer[21];
	int32 bufferStart = 20;
	buffer[bufferStart] = L'\0';


	if (num < 0)
	{
		num = -num;

		do
		{
			buffer[--bufferStart] = L'0' + (num % 10);
			num /= 10;
		} while (num != 0);

		buffer[--bufferStart] = L'-';
	}
	else
	{
		do
		{
			buffer[--bufferStart] = L'0' + (num % 10);
			num /= 10;
		} while (num != 0);
	}


	return (buffer + bufferStart);
}

void LowerStr(const char* inStr, char* outStr)
{
	int64 Cnt = 0;

	while(true)
	{
		outStr[Cnt++] = tolower(*inStr);
		if (*inStr == '\0') break;
		inStr++;
	}
}

void LowerStr(const utf16* inStr, utf16* outStr)
{
	int64 Cnt = 0;

	while (true)
	{
		outStr[Cnt++] = std::tolower(*inStr);
		if (*inStr == '\0') break;
		inStr++;
	}
}

void LowerStr(SS::StringW& InStr)
{
	utf16* InStrRaw = const_cast<utf16*>(InStr.C_Str());

	while (true)
	{
		*InStrRaw = std::tolower(*InStrRaw);
		if (*InStrRaw == L'\0') break;
		InStrRaw++;
	}
}

void ExtractFileNameFromPath(SS::StringW& OutStr, const utf16* inFilePath, bool bIncludeSuffix)
{
	OutStr.Clear();

	const utf16* fileNameStart = wcsrchr(inFilePath, L'/');
	if(fileNameStart == nullptr)
	{
		fileNameStart = wcsrchr(inFilePath, L'\\');
	}

	if(fileNameStart == nullptr)
	{
		return;
	}

	OutStr = fileNameStart + 1;

	if (bIncludeSuffix == false)
	{
		int32 CutOutLen = wcsrchr(OutStr.C_Str(), L'.') - OutStr.C_Str();
		OutStr.CutOut(CutOutLen);
	}
}
