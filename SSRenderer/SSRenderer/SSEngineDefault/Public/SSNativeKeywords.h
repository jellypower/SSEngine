#pragma once
#ifdef _WINDOWS

typedef long long int			int64;
typedef int						int32;
typedef short					int16;
typedef char					int8;

typedef unsigned long long		uint64;
typedef unsigned int			uint32;
typedef unsigned short			uint16;
typedef unsigned char			uint8;

typedef unsigned char			byte;

typedef wchar_t					utf16;
typedef char					utf8;

constexpr uint32 SS_UINT32_MAX = 0xffffffff;
constexpr int64 SS_INT64_MAX = 0x7FFFFFFFFFFFFFFF;
constexpr int64 INVALID_IDX = -1;

#define FORCEINLINE __forceinline
#define PATH_LEN_MAX 520
#define ASSET_NAME_LEN_MAX 128

#endif