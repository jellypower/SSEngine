#include "pch.h"
#include "DX12GALRIDirectionalLightShadowMapMetadata.h"

#include "Private/DX12/DX12CommonUtils/DX12ConstantBufferResourcePage.h"
#include "Public/GALConstantBufferAccessorTypes/CBARenderEnvParam.h"
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"

#include "SSRenderer/Public/RenderInstance/Descriptors/LightDesc.h"
#include "SSRenderer/Public/RenderInstance/Light/IRenderLightDirectional.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Public/GALRenderTarget/GALRenderTarget.h"

DX12GALRIDirectionalLightShadowMapMetadata::DX12GALRIDirectionalLightShadowMapMetadata(
	DX12GALRenderDevice* InRenderDevice
	, IRenderLightDirectional* InOwnerRenderLight)
{
	_OwnerRenderInstance = InOwnerRenderLight;

	// Shadow Map
	{
		const RenderLightDirectionalDesc& Desc = InOwnerRenderLight->GetDirectionalLightDesc();
		GALRenderTargetDesc ShadowMapDesc;
		ShadowMapDesc.ResourceWidth = Desc.ShadowMapSize.X;
		ShadowMapDesc.ResourceHeight = Desc.ShadowMapSize.Y;
		ShadowMapDesc.ScissorRectSize.Min = Vector2f(0, 0);
		ShadowMapDesc.ScissorRectSize.Max = Desc.ShadowMapSize;
		ShadowMapDesc.DrawBoxSize.LeftTop = Vector2f(0, 0);
		ShadowMapDesc.DrawBoxSize.WidthHeight = Desc.ShadowMapSize;
		ShadowMapDesc.DrawBoxSize.MinDepth = 0.f;
		ShadowMapDesc.DrawBoxSize.MaxDepth = 1.f;
		ShadowMapDesc.Format = ERTColorFormat::D32_FLOAT;
		ShadowMapDesc.InitialResourceState = EResourceStateType::Common;
		ShadowMapDesc.bUseSRV = true;
		_ShadowMap = InRenderDevice->CreateDepthStencilView(ShadowMapDesc, L"DX12GALRIDirectionalLightShadowMapMetadata");
	}

	// Constant Buffer
	{
		_OwnerRenderDevice = InRenderDevice;
		ID3D12Device5* D3DDevice = _OwnerRenderDevice->GetD3DDevice();
		SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();


		_ShadowMapCBChunk = ConstantBufferAllocator->AllocChunk(sizeof(CBARenderEnvParam),
			"DX12GALRIDirectionalLightShadowMapMetadata");
		DX12ConstantBufferResourcePage* RenderEnvCBPage = (DX12ConstantBufferResourcePage*)_ShadowMapCBChunk.PageContent;
		_ShadowMapCBSysMemAddr = reinterpret_cast<CBARenderEnvParam*>(RenderEnvCBPage->ResourceSysMem + _ShadowMapCBChunk.ChunkOffset);
		_ShadowMapCBGPUMemAddr = RenderEnvCBPage->D3D12Resource->GetGPUVirtualAddress() + _ShadowMapCBChunk.ChunkOffset;
	}
}

DX12GALRIDirectionalLightShadowMapMetadata::~DX12GALRIDirectionalLightShadowMapMetadata()
{
	if (_ShadowMap != nullptr)
	{
		delete _ShadowMap;
		_ShadowMap = nullptr;
	}

	SSCustomMemChunkAllocator* ConstantBufferAllocator = _OwnerRenderDevice->GetConstantBufferResourceAllocator();
	ConstantBufferAllocator->ReleaseChunk(_ShadowMapCBChunk);
}

ERenderInstanceType DX12GALRIDirectionalLightShadowMapMetadata::GetMetadataRenderInstanceType()
{
    return ERenderInstanceType::Light;
}

ELightType DX12GALRIDirectionalLightShadowMapMetadata::GetLightType() const
{
    return ELightType::Directional;
}

GALRenderTarget* DX12GALRIDirectionalLightShadowMapMetadata::GetShadowMap() const
{
	return _ShadowMap;
}
