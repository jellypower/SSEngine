#include "pch.h"


#include "SSEngineDefault/Public/SSContainer/BasicHashFunctionCollection.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "PSOWrapper.h"


bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs)
{
	bool bResult = lhs.VSName == rhs.VSName;
	bResult = bResult && lhs.PSName == rhs.PSName;
	bResult = bResult && lhs.LayoutType == rhs.LayoutType;
	bResult = bResult && lhs.RootSignatureType == rhs.RootSignatureType;
	bResult = bResult && lhs.DSColorFormat == rhs.DSColorFormat;

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

uint32 HashValue(const PipelineDesc& inValue)
{
	uint32 PSHashValue = HashValue(inValue.PSName);
	uint32 VSHashValue = HashValue(inValue.VSName);
	uint32 LayoutTypeHashValue = (uint32)inValue.LayoutType;
	uint32 RootsignatureTypeHashValue = (uint32)inValue.RootSignatureType;
	uint32 ui32DSColorFormat = (uint32)inValue.DSColorFormat;


	uint32 RTColorFormatHashValue = inValue.NumRenderTarget;
	for (int32 i=0;i<inValue.NumRenderTarget;i++)
	{
		RTColorFormatHashValue += (uint32)inValue.RTColorFormats[i];
	}

	int64 HashResult = (PSHashValue ^ VSHashValue) >> 1
		+ LayoutTypeHashValue + RootsignatureTypeHashValue + RTColorFormatHashValue + ui32DSColorFormat;

	return HashResult;
}


PSOWrapper::PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool)
	: _pipelineDesc(InPipelineDesc), _OwnerPSOPool(InOwnerPSOPool)
{
}

PSOWrapper::~PSOWrapper()
{
}
