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

int64 HashValue(const PipelineDesc& inValue)
{
	int64 PSHashValue = HashValue(inValue.PSName.GetDirectValue());
	int64 VSHashValue = HashValue(inValue.VSName.GetDirectValue());


	int64 LayoutTypeHashValue = (int64)inValue.LayoutType;
	int64 RootsignatureTypeHashValue = (int64)inValue.RootSignatureType;

	int64 HashResult = (PSHashValue ^ VSHashValue) >> 1 + LayoutTypeHashValue + RootsignatureTypeHashValue;

	return HashResult;
}


PSOWrapper::PSOWrapper(const PipelineDesc& pipelineDesc)
	: _pipelineDesc(pipelineDesc)
{
}

PSOWrapper::~PSOWrapper()
{
}
