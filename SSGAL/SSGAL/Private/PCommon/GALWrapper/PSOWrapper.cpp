#include "pch.h"


#include "SSEngineDefault/Public/SSContainer/BasicHashFunctionCollection.h"
#include "SSEngineDefault/Public/SSNativeKeywords.h"
#include "PSOWrapper.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataBase.h"

#include "Public/GALRenderTarget/GALRenderTarget.h"


PSOWrapper::PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool)
	: _pipelineDesc(InPipelineDesc), _OwnerPSOPool(InOwnerPSOPool)
{
}

PSOWrapper::~PSOWrapper()
{
}

PipelineDesc ConstructPSODescToDrawMesh(EMeshType InMeshType, EMaterialType InMtlType, int32 NumRenderTarget,
	GALRenderTarget* const* InRenderTargets, GALRenderTarget* InDSV)
{
	PipelineDesc NewPipelineDesc;

	switch (InMeshType)
	{
	case EMeshType::Rigid:
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SMToDefaultPSInput";
		break;

	case EMeshType::Skinned:
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SKMToDefaultPSInput";
		break;

	default:
		SS_ASSERT(false);
		break;
	}

	switch (InMtlType)
	{
	case EMaterialType::DefaultPBR:
		NewPipelineDesc.PSName = L"PS_DeferredShadeGBufferOutput";
		NewPipelineDesc.RootSignatureType = ERootSignatureType::SS_DEFAULT_PBR;
		break;
	default:
		SS_ASSERT(false);
		break;
	}

	NewPipelineDesc.NumRenderTarget = NumRenderTarget;
	for (int32 i = 0; i < NumRenderTarget; i++)
	{
		NewPipelineDesc.RTColorFormats[i] = InRenderTargets[i]->GetRTColorFormat();
	}
	NewPipelineDesc.DSColorFormat = InDSV->GetRTColorFormat();

	return NewPipelineDesc;
}

PipelineDesc ConstructPSODescToDrawShadow(EMeshType InMeshType)
{
	PipelineDesc NewPipelineDesc;

	switch (InMeshType)
	{
	case EMeshType::Rigid:
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SMToDefaultPSInput";
		break;

	case EMeshType::Skinned:
		NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT;
		NewPipelineDesc.VSName = L"VS_SKMToDefaultPSInput";
		break;

	default:
		SS_ASSERT(false);
		break;
	}

	NewPipelineDesc.DSColorFormat = ERTColorFormat::D32_FLOAT;
	NewPipelineDesc.RootSignatureType = ERootSignatureType::Shadow;

	return NewPipelineDesc;
}

PipelineDesc ConstructPSOToDeferredShading()
{
	PipelineDesc NewPipelineDesc;
	NewPipelineDesc.VSName = "VS_FullScreenQuad";
	NewPipelineDesc.PSName = "PS_DeferredShading";
	NewPipelineDesc.LayoutType = EInputLayoutType::SS_INPUTLAYOUT_NULL;
	NewPipelineDesc.RootSignatureType = ERootSignatureType::DeferredShading;
	NewPipelineDesc.NumRenderTarget = 1;
	NewPipelineDesc.RTColorFormats[0] = ERTColorFormat::R8G8B8A8_UNORM;
	NewPipelineDesc.DSColorFormat = ERTColorFormat::Unknown;
	return NewPipelineDesc;
}

PipelineDesc ConstructPSOToDrawSkyMap()
{
	PipelineDesc NewPipelineDesc;
	NewPipelineDesc.VSName = "VS_SMToDefaultPSInput";
	NewPipelineDesc.PSName = "PS_SkyMap";
	NewPipelineDesc.LayoutType = EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT;
	NewPipelineDesc.RootSignatureType = ERootSignatureType::SkyMap;
	NewPipelineDesc.NumRenderTarget = 1;
	NewPipelineDesc.RTColorFormats[0] = ERTColorFormat::R8G8B8A8_UNORM;
	NewPipelineDesc.DSColorFormat = ERTColorFormat::Unknown;
	return NewPipelineDesc;
}
