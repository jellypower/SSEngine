#pragma once
#include "ScopedProfile.h"


#define BUILD_ENABLE_PROFILE 1

#if BUILD_ENABLE_PROFILE

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define UNIQUE_NAME(base) CONCAT(base, __LINE__)


#define SCOPE_PROFILE(NAME) \
	static const SS::SHasherW UNIQUE_NAME(__ProfName_) = L#NAME; \
	ScopedProfile UNIQUE_NAME(__Prof_)(UNIQUE_NAME(__ProfName_))

#define SCOPE_PROFILE_INDEXED(NAME, IDX) \
	ScopedProfileIndexed UNIQUE_NAME(__Prof_)(L#NAME, IDX)

#else

#define SCOPE_PROFILE(NAME)

#endif
