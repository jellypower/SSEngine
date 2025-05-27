#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class ERootSignatureType : int32
{
	NONE = -1,

	SS_TEMP_ROOTSIGNATURE,
	SS_DEFAULT_PBR,
	
	COUNT
};

FORCEINLINE const utf16* ToUtf16Str(ERootSignatureType e)
{
	switch (e)
	{
	case ERootSignatureType::NONE:				return L"ERootSignatureType::NONE";
	case ERootSignatureType::SS_DEFAULT_PBR:	return L"ERootSignatureType::SS_DEFAULT_PBR";
	default:									return L"ERootSignatureType::unknown";
	}
}

class RootSignatureWrapper : public INoncopyable
{
private:
	ERootSignatureType _rootSignatureType = ERootSignatureType::NONE;

public:
	RootSignatureWrapper(ERootSignatureType rootSignatureType);
	virtual ~RootSignatureWrapper();

	virtual bool IsValid() const = 0;
	ERootSignatureType GetRootSignatureType() const { return _rootSignatureType; }
};

