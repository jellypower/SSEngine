#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "RootSignatureWrapper.h"


enum class EInputLayoutType : int32
{
	NONE = 0,

	SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT = 1,
	SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT = 2,
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
	EInputLayoutType	LayoutType;
	ERootSignatureType	RootSignatureType;
};

bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs);
int64 HashValue(const PipelineDesc& inValue);


class PSOWrapper : public INoncopyable
{
private:
	PipelineDesc _pipelineDesc;

public:
	PSOWrapper(const PipelineDesc& pipelineDesc);
	virtual ~PSOWrapper();

	virtual bool IsValid() const = 0;
	const PipelineDesc& GetPipelineDesc() const { return _pipelineDesc; }
};
