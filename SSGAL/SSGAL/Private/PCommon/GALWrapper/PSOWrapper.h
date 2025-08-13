#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"
#include "RootSignatureWrapper.h"
#include "Public/SSGALInlineSettings.h"
#include "Public/GALRenderTarget/GALRTCommonEnums.h"


class GALRenderTarget;
enum class EMaterialType;
enum class EMeshType;
class PSOPool;

enum class EInputLayoutType : int32
{
	NONE = 0,

	SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT,
	SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT,
	ComputeShader,
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
};

bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs);
uint32 HashValue(const PipelineDesc& inValue);


class PSOWrapper : public INoncopyable
{
private:
	PSOPool* _OwnerPSOPool = nullptr;
	PipelineDesc _pipelineDesc;

public:
	PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool);
	virtual ~PSOWrapper();

	virtual bool IsValid() const = 0;
	const PipelineDesc& GetPipelineDesc() const { return _pipelineDesc; }
};



PipelineDesc ConstructPSODescToDrawMesh(
	EMeshType InMeshType,
	EMaterialType InMtlType, 
	int32 NumRenderTarget,
	GALRenderTarget* const* InRenderTargets,
	GALRenderTarget* InDSV);

PipelineDesc ConstructPSODescToDrawShadow(
	EMeshType InMeshType
);