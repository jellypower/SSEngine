#pragma once
#include "GALRenderDeviceContext.h"
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/SSContainer/Allocators/InlineAllocator.h"
#include "SSGAL/Public/SSGALInlineSettings.h"

enum class ERTColorFormat : int32;
class IRenderer;
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
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;

public:
	IRenderer* GetOwnerRenderer() const { return _OwnerRenderer; }

	GALShaderPool* GetShaderPool() const { return _ShaderPool; }
	SSCustomMemChunkAllocator* GetConstantBufferResourceAllocator() const { return _ConstantBufferResourceAllocator; }
	SSCustomMemChunkAllocator* GetDescriptorTableAllocator() const { return _DescriptorTableAllocator; }

	GALRenderTarget* GetDefaultViewportRenderTarget() const { return _DefaultViewportRenderTarget; }
	uint64 GetCurFrameCnt() const { return _CurFrameCnt; }

	virtual ERenderDevicePlatnform GetRenderDevicePlatform() const = 0;


public:
	virtual void BindRendererXXX(IRenderer* InOwnerRenderer) = 0;


	virtual GALRenderDeviceContext* CreateRenderDeviceContext() = 0;
	virtual GALRenderTarget* CreateRenderTarget(const GALRenderTargetDesc& Desc, const utf16* ResourceName = nullptr) = 0;
	virtual GALCPUReadableTexture* CreateCPUReadableTexture(ERTColorFormat InColorFormat, Vector2i32 InWidthHeight, int32 Pitch, const utf16* ResourceName = nullptr) = 0;

	virtual void ExecuteRenderContext(GALRenderDeviceContext* DeviceContext) = 0;


protected:
	virtual void WaitForFence() = 0;
	virtual void FenceFrame() = 0;

protected:
	IRenderer* _OwnerRenderer = nullptr;

	GALShaderPool* _ShaderPool = nullptr;
	SSCustomMemChunkAllocator* _ConstantBufferResourceAllocator = nullptr;
	SSCustomMemChunkAllocator* _DescriptorTableAllocator = nullptr;


	GALRenderTarget* _DefaultViewportRenderTarget = nullptr;

	int32 _NestedFrameCnt = SWAP_CHAIN_FRAME_COUNT;

	uint64 _CurFrameCnt = 0;

	SS::PooledList<GALRenderDeviceContext*, SS::InlineAllocator<10>> _ExecutedDeviceContext;
};