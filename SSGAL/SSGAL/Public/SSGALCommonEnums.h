#pragma once
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "SSEngineDefault/Public/SSDebugLogger.h"

enum class EResourceStateType : uint8
{
	None,

	Common,
	CopySrc,
	CopyDest,
	RenderTarget,
	Present,
	DepthWrite,
};

enum class ERootSignatureType : int32
{
	NONE = -1,

	SS_TEMP_ROOTSIGNATURE,
	SS_DEFAULT_PBR,
	Shadow,
	DeferredShading,
	SkyMap,
	DebugWire,

	// CS_TEMP, // TODO: Implementation

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
	case ERootSignatureType::DeferredShading:	return L"ERootSignatureType::ExecuteDeferredShading";
	case ERootSignatureType::SkyMap:	return L"ERootSignatureType::SkyMap";
	case ERootSignatureType::DebugWire:	return L"ERootSignatureType::DebugWire";
	default:
		SS_ASSERT(false);
		return L"ERootSignatureType::unknown";
	}
}