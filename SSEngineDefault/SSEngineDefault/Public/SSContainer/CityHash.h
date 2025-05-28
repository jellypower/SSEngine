#pragma once
#include "SSEngineDefault/ModuleExportKeyword.h"

#include "SSUtilityContainer.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"


typedef SS::pair<uint64, uint64> uint128;

inline uint64 Uint128Low64(const uint128& x) { return x.first; }
inline uint64 Uint128High64(const uint128& x) { return x.second; }

SSENGINEDEFAULT_MODULE_NATIVE uint64 CityHash64(const char* buf, size_t len);
SSENGINEDEFAULT_MODULE_NATIVE uint64 CityHash64WithSeed(const char* buf, size_t len, uint64 seed);
SSENGINEDEFAULT_MODULE_NATIVE uint64 CityHash64WithSeeds(const char* buf, size_t len, uint64 seed0, uint64 seed1);

SSENGINEDEFAULT_MODULE_NATIVE uint128 CityHash128(const char* s, size_t len);
SSENGINEDEFAULT_MODULE_NATIVE uint128 CityHash128WithSeed(const char* s, size_t len, uint128 seed);

SSENGINEDEFAULT_MODULE_NATIVE uint32 CityHash32(const char* buf, size_t len);

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64 Hash128to64(const uint128& x) {
    // Murmur-inspired hashing.
    const uint64 kMul = 0x9ddfea08eb382d69ULL;
    uint64 a = (Uint128Low64(x) ^ Uint128High64(x)) * kMul;
    a ^= (a >> 47);
    uint64 b = (Uint128High64(x) ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}