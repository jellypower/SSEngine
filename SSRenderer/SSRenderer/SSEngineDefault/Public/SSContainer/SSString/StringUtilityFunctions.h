#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"

namespace SS
{
	class StringW;
}

int32 UTF16StrLenOfCharStr(const char* charStr, int32 charLen = INVALID_IDX);

// return: written string length
int32 CharStrToUTF16Str(const char* charStr, int32 charLen, utf16* outUtf16Str, int32 utf16StrBufferSize);

// return: written string length
int32 UTF16StrToCharStr(const utf16* utf16Str, int32 utf16StrLen, char* outCharStr, int32 charLen);

// return: written bytes num
int32 UTF16StrToUtf8Str(const utf16* utf16Str, int32 utf16StrLen, utf8* outUtf8Str, int32 utf8BufferSize);

SS::StringW IntToString(int32 num);

void LowerStr(const char* inStr, char* outStr);
void LowerStr(const utf16* inStr, utf16* outStr);
void LowerStr(SS::StringW& InStr);

void ExtractFileNameFromPath(SS::StringW& OutStr, const utf16* inFilePath);