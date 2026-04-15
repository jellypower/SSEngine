#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SHasher/Internal/HasherPoolAccesFunc.h"

#include "SSEngineDefault/Public/WInWrapper/WinWrapper.h"

#include "HasherPoolCreationFunction.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"
#include "SSEngineDefault/Public/SSEngineInlineSettings.h"
#include "SSEngineDefault/Public/SHasher/IHasherPool.h"
#include "SSEngineDefault/Public/SSContainer/CityHash.h"

IHasherPool* g_HasherPool = nullptr;

volatile long g_AssertGlobalHasherPoolMustBeUnique = 0;

const HasherPoolNode* FindOrAddHasherNode(const utf16* InStr)
{
	if (g_HasherPool == nullptr)
	{
		long OldValue = _InterlockedCompareExchange(&g_AssertGlobalHasherPoolMustBeUnique, 1, 0);
		if (OldValue != 0)
		{
			SS_INTERRUPT();
		}

		if (g_AssertGlobalHasherPoolMustBeUnique != 1)
		{
			SS_INTERRUPT();
		}

		g_HasherPool = CreateHasherPool(SHASHER_DEFAULT_POOL_SIZE);
	}

	uint32 StrLen = wcslen(InStr);
	if (StrLen > SHASHER_STRLEN_MAX)
	{
		SS_INTERRUPT();
	}

	if (StrLen <= 0)
	{
		return nullptr;
	}

	uint32 HashedValue = CityHash32(reinterpret_cast<const char*>(InStr), StrLen * (sizeof(utf16) / sizeof(char)));
	return g_HasherPool->FindOrAddHasherValue(InStr, StrLen, HashedValue);
}

void DestroyGlobalHasherPool()
{
	delete g_HasherPool;
	g_HasherPool = nullptr;
}
