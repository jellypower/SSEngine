#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSEngineDefault/Public/SSNativeKeywords.h"

namespace SS
{
	class StringW;
}

SSENGINEDEFAULT_MODULE int32 UTF16StrLenOfCharStr(const char* charStr, int32 charLen = INVALID_IDX);

// return: written string length
SSENGINEDEFAULT_MODULE int32 CharStrToUTF16Str(const char* charStr, int32 charLen, utf16* outUtf16Str, int32 utf16StrBufferSize);

// return: written string length
SSENGINEDEFAULT_MODULE int32 UTF16StrToCharStr(const utf16* utf16Str, int32 utf16StrLen, char* outCharStr, int32 charLen);

// return: written bytes num
SSENGINEDEFAULT_MODULE int32 UTF16StrToUtf8Str(const utf16* utf16Str, int32 utf16StrLen, utf8* outUtf8Str, int32 utf8BufferSize);

SSENGINEDEFAULT_MODULE SS::StringW IntToString(int32 num);

SSENGINEDEFAULT_MODULE void LowerStr(const char* inStr, char* outStr);
SSENGINEDEFAULT_MODULE void LowerStr(const utf16* inStr, utf16* outStr);
SSENGINEDEFAULT_MODULE void LowerStr(SS::StringW& InStr);

SSENGINEDEFAULT_MODULE void ExtractFileNameFromPath(SS::StringW& OutStr, const utf16* inFilePath);