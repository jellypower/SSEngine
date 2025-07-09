#include "pch.h"


#include "SSEngineDefault/Public/SSContainer/BasicHashFunctionCollection.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "PSOWrapper.h"


bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs)
{
	bool compareResult[4];

	compareResult[0] = lhs.VSName == rhs.VSName;
	compareResult[1] = lhs.PSName == rhs.PSName;
	compareResult[2] = lhs.LayoutType == rhs.LayoutType;
	compareResult[3] = lhs.RootSignatureType == rhs.RootSignatureType;


	return
		compareResult[0] &&
		compareResult[1] &&
		compareResult[2] &&
		compareResult[3];

}

uint32 HashValue(const PipelineDesc& inValue)
{
	uint32 PSHashValue = HashValue(inValue.PSName);
	uint32 VSHashValue = HashValue(inValue.VSName);


	uint32 LayoutTypeHashValue = (uint32)inValue.LayoutType;
	uint32 RootsignatureTypeHashValue = (uint32)inValue.RootSignatureType;
	uint32 ui32RTColorFormat = (uint32)inValue.RTColorFormat;
	uint32 ui32DSColorFormat = (uint32)inValue.DSColorFormat;

	int64 HashResult = (PSHashValue ^ VSHashValue) >> 1
		+ LayoutTypeHashValue + RootsignatureTypeHashValue + ui32RTColorFormat + ui32DSColorFormat;

	return HashResult;
}


PSOWrapper::PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool)
	: _pipelineDesc(InPipelineDesc), _OwnerPSOPool(InOwnerPSOPool)
{
}

PSOWrapper::~PSOWrapper()
{
}
