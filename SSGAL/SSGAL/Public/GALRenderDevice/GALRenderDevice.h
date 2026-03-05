#pragma once
#include "GALRenderDeviceContext.h"
#include "SSEngineDefault/Public/INoncopyable.h"

class GALPPCDeferredShading;
enum class ERTColorFormat : int32;
class IRenderer;
class ICommonRenderAssetSet;
class GALShaderPool;
struct GALRenderTargetDesc;
class SSCustomMemChunkAllocator;
class GALRenderTarget;

enum class ERenderDevicePlatnform : uint8
{
	None = 0,

	DX12Raster,
};

// GraphicsAPI Abstraction Layer Device
class GALRenderDevice : public INoncopyable
{
public:
	IRenderer* GetOwnerRenderer() const { return _OwnerRenderer; }
	ICommonRenderAssetSet* GetCommonRenderAssetSet() const { return _CachedCommonRenderAssetSet; }

	GALShaderPool* GetShaderPool() const { return _ShaderPool; }
	SSCustomMemChunkAllocator* GetConstantBufferResourceAllocator() const { return _ConstantBufferResourceAllocator; }
	SSCustomMemChunkAllocator* GetDescriptorTableAllocator() const { return _DescriptorTableAllocator; }
	SSCustomMemChunkAllocator* GetDescriptorTableAllocatorForTex() const { return _DescriptorTableAllocatorForTex; }

	bool IsDebugEnabled() const { return _EnableDebug; }

	virtual ERenderDevicePlatnform GetRenderDevicePlatform() const = 0;

public:
	
	virtual void BindRendererXXX(IRenderer* InOwnerRenderer) = 0;

	virtual GALRenderDeviceContext* CreateRenderDeviceContext() = 0;
	virtual GALRenderTarget* CreateRenderTarget(const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr) = 0;
	virtual GALRenderTarget* CreateDepthStencilView(const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr) = 0;
	virtual GALCPUReadableTexture* CreateCPUReadableTexture(ERTColorFormat InColorFormat, Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName = nullptr) = 0;
	virtual GALPPCDeferredShading* CreateDeferredShadingPostProcessContext() = 0;

protected:
	IRenderer* _OwnerRenderer = nullptr;
	ICommonRenderAssetSet* _CachedCommonRenderAssetSet = nullptr;

	GALShaderPool* _ShaderPool = nullptr;
	SSCustomMemChunkAllocator* _ConstantBufferResourceAllocator = nullptr;
	SSCustomMemChunkAllocator* _DescriptorTableAllocator = nullptr;
	SSCustomMemChunkAllocator* _DescriptorTableAllocatorForTex = nullptr;

	bool _EnableDebug = false;
};