#pragma once
#include "Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSGAL/Public/SSGALCommonEnums.h"
#include "SSEngineDefault/Public/SSContainer/BasicHashFunctionCollection.h"


class GALRenderTarget;
enum class EMaterialType;
enum class EMeshType;
class PSOPool;

enum class EInputLayoutType : int32
{
	NONE = 0,

	SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT,
	SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT,

	SS_INPUTLAYOUT_NULL,
};

enum class ERasterizerState : int32
{
	NONE = 0,

	SS_DEFAULT_RASTERIZE_STATE = 1,
};

enum class EBlendState : int32
{
	NONE = 0,

	SS_DEFAULT_BLEND_STATE = 1,
};




struct PipelineDesc
{
	SS::SHasherW		VSName;
	SS::SHasherW		PSName;
	SS::SHasherW		CSName;
	EInputLayoutType	LayoutType = EInputLayoutType::NONE;
	ERootSignatureType	RootSignatureType = ERootSignatureType::NONE;
	ERTColorFormat		DSColorFormat = ERTColorFormat::None; // DepthStancilColorFormat
	int32				NumRenderTarget = 0;
	ERTColorFormat		RTColorFormats[RT_NUM_MAX] = { ERTColorFormat::None, }; // RenderTargetColorFormat
	bool				bUseTwoSideRender = false;
	bool				bUseWireFrame = false;
};

inline bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs)
{
	bool bResult = lhs.VSName == rhs.VSName;
	bResult = bResult && lhs.PSName == rhs.PSName;
	bResult = bResult && lhs.CSName == rhs.CSName;
	bResult = bResult && lhs.LayoutType == rhs.LayoutType;
	bResult = bResult && lhs.RootSignatureType == rhs.RootSignatureType;
	bResult = bResult && lhs.DSColorFormat == rhs.DSColorFormat;
	bResult = bResult && lhs.bUseTwoSideRender == rhs.bUseTwoSideRender;
	bResult = bResult && lhs.bUseWireFrame == rhs.bUseWireFrame;

	if (bResult == false)
	{
		return bResult;
	}

	bResult = bResult && lhs.NumRenderTarget == rhs.NumRenderTarget;
	for (int i = 0; i < lhs.NumRenderTarget; i++)
	{
		bResult = bResult && lhs.RTColorFormats[i] == rhs.RTColorFormats[i];
	}

	return bResult;
}

inline uint32 HashValue(const PipelineDesc& inValue)
{
	uint32 PSHashValue = HashValue(inValue.PSName);
	uint32 VSHashValue = HashValue(inValue.VSName);
	uint32 CSHashValue = HashValue(inValue.CSName);
	uint32 LayoutTypeHashValue = (uint32)inValue.LayoutType;
	uint32 RootsignatureTypeHashValue = (uint32)inValue.RootSignatureType;
	uint32 ui32DSColorFormat = (uint32)inValue.DSColorFormat;
	uint32 uiUseTwoSideRender = inValue.bUseTwoSideRender ? 1 : 0;
	uint32 uiUseWireFrame = inValue.bUseWireFrame ? 1 : 0;


	uint32 RTColorFormatHashValue = inValue.NumRenderTarget;
	for (int32 i = 0; i < inValue.NumRenderTarget; i++)
	{
		RTColorFormatHashValue += (uint32)inValue.RTColorFormats[i];
	}

	int64 HashResult = (PSHashValue ^ VSHashValue ^ CSHashValue) >> 1 +
		LayoutTypeHashValue + RootsignatureTypeHashValue + RTColorFormatHashValue + ui32DSColorFormat +
		uiUseTwoSideRender + uiUseWireFrame;

	union {
		struct {
			uint32 valueH; // 해쉬 상위 32비트
			uint32 valueL; // 해쉬 하위 32비트
		};
		int64 valueX; // 해쉬 64비트 전체값
	};

	valueX = HashResult;
	return valueH ^ valueL;
}
