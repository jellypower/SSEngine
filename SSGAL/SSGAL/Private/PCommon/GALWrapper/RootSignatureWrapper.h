#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

class RootSignaturePool;

enum class ERootSignatureType : int32
{
	NONE = -1,

	SS_TEMP_ROOTSIGNATURE,
	SS_DEFAULT_PBR,
	Shadow,
	CS_Deferred,

	COUNT
};


FORCEINLINE const utf16* ToUtf16Str(ERootSignatureType e)
{
	switch (e)
	{
	case ERootSignatureType::NONE: return L"ERootSignatureType::NONE";
	case ERootSignatureType::SS_TEMP_ROOTSIGNATURE: return L"ERootSignatureType::SS_TEMP_ROOTSIGNATURE";
	case ERootSignatureType::SS_DEFAULT_PBR:	return L"ERootSignatureType::SS_DEFAULT_PBR";
	case ERootSignatureType::Shadow:	return L"ERootSignatureType::Shadow";
	case ERootSignatureType::CS_Deferred:	return L"ERootSignatureType::CS_Deferred";
	default:
		SS_ASSERT(false);
		return L"ERootSignatureType::unknown";
	}
}

class RootSignatureWrapper : public INoncopyable
{
protected:
	ERootSignatureType _rootSignatureType = ERootSignatureType::NONE;
	RootSignaturePool* _OwnerRenderSignaturePool = nullptr;

public:
	RootSignatureWrapper(ERootSignatureType rootSignatureType, RootSignaturePool* InOwnerRootSignaturePool);
	virtual ~RootSignatureWrapper();

	virtual bool IsValid() const = 0;
	ERootSignatureType GetRootSignatureType() const { return _rootSignatureType; }
};

