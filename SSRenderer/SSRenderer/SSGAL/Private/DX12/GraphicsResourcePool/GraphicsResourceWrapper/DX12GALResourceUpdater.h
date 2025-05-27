#pragma once
#include <d3d12.h>
#include "SSGAL/Public/GraphicsResourcePool/GALResourceUpdater.h"

#include "SSEngineDefault/Public/SSContainer/PooledList.h"

class DX12GALRenderDevice;
class DX12GALRenderDeviceContext;

class DX12GALResourceUpdater : public GALResourceUpdater
{
public:
	DX12GALResourceUpdater(DX12GALRenderDevice* InRenderDevice, DX12GALRenderDeviceContext* InRenderDeviceContext);
	virtual ~DX12GALResourceUpdater();

	virtual void ResetUpdateBuffer() override;

	HRESULT UpdateBuffer(
		ID3D12GraphicsCommandList* CommandList,
		ID3D12Resource* Dest,
		const void* Src,
		int32 Size,
		D3D12_RESOURCE_STATES FromState,
		D3D12_RESOURCE_STATES ToState);

private:
	ID3D12Resource* RentUpdateBuffer(int32& OutBufferStartOffset, int32 BufferSize);

private:
	SS::PooledList<ID3D12Resource*> _DefaultUploadBuffers;
	int32 _CurDefaultBufferIdx = 0;
	int32 _CurBufferOffset = 0;


	// 4MB, 8MB, 16MB, 32MB, 64MB
	static constexpr int32 LARGE_UPLOADBUFFER_SIZE_MIN = 1024 * 1024 * 2 * 2;
	static constexpr int32 LARGE_UPLOADBUFFER_TYPE_CNT = 5;
	SS::PooledList<ID3D12Resource*> _LargeUploadBuffers[LARGE_UPLOADBUFFER_TYPE_CNT];
	int32 _CurLargeUploadBufferIndices[LARGE_UPLOADBUFFER_TYPE_CNT];


};
