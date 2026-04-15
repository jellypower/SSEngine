#include "pch.h"


#include "DX12GALResourceUpdater.h"

#include "SSEngineDefault/Public/RawProfiler/ScopeProfMacro.h"

#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDeviceContext.h"

DX12GALResourceUpdater::DX12GALResourceUpdater(DX12GALRenderDevice* InRenderDevice, DX12GALRenderDeviceContext* InRenderDeviceContext)
	: GALResourceUpdater(InRenderDevice, InRenderDeviceContext)
{
	HRESULT hr = S_OK;
	ID3D12Device5* D3DDevice = InRenderDevice->GetD3DDevice();


	_DefaultUploadBuffers.Reserve(GAL_INITIAL_DEFAULT_RESOURCEUPDATER_PAGECNT * 2);

	// 4K 텍스쳐 해상도 사이즈
	CD3DX12_HEAP_PROPERTIES UploadHeapTypeProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GAL_DEFAULT_RESOURCEUUPDATER_PAGESIZE);

	for (int32 i = 0; i < GAL_INITIAL_DEFAULT_RESOURCEUPDATER_PAGECNT; i++)
	{
		ID3D12Resource* NewUpdateBuffer = nullptr;

		hr = D3DDevice->CreateCommittedResource(
			&UploadHeapTypeProp,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&NewUpdateBuffer));
		NewUpdateBuffer->SetName(L"ResourceUpdater");

		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}

		_DefaultUploadBuffers.PushBack(NewUpdateBuffer);
	}

	for (int32 i = 0; i < LARGE_UPLOADBUFFER_TYPE_CNT; i++)
	{
		_CurLargeUploadBufferIndices[i] = 0;
	}
}

DX12GALResourceUpdater::~DX12GALResourceUpdater()
{
	for (ID3D12Resource* UploadBufferItem : _DefaultUploadBuffers)
	{
		UploadBufferItem->Release();
	}
	_DefaultUploadBuffers.Clear();


	for (SS::PooledList<ID3D12Resource*>& BufferList: _LargeUploadBuffers)
	{
		for (ID3D12Resource* ResourceItem : BufferList)
		{
			ResourceItem->Release();
		}

		BufferList.Clear();
	}
}

void DX12GALResourceUpdater::ResetUpdateBuffer()
{
	for (int32 i = 0; i < LARGE_UPLOADBUFFER_TYPE_CNT; i++)
	{
		_CurLargeUploadBufferIndices[i] = 0;
	}

	_CurDefaultBufferIdx = 0;
	_CurBufferOffset = 0;
}

HRESULT DX12GALResourceUpdater::UpdateBuffer(
	ID3D12GraphicsCommandList* CommandList,
	ID3D12Resource* Dest,
	const void* Src,
	int32 Size,
	D3D12_RESOURCE_STATES FromState,
	D3D12_RESOURCE_STATES ToState)
{
	HRESULT hr = S_OK;
	byte* pUploadBufferData = nullptr;
	CD3DX12_RANGE writeRange(0, 0); // We do not intend to read from this resource on the CPU.

	int32 SrcBufferOffset = 0;
	ID3D12Resource* UpdateResourceBuffer = RentUpdateBuffer(SrcBufferOffset, Size);

	hr = UpdateResourceBuffer->Map(0, &writeRange, reinterpret_cast<void**>(&pUploadBufferData));
	if (FAILED(hr))
	{
		DEBUG_BREAK();
		return hr;
	}
	memcpy(pUploadBufferData + SrcBufferOffset, Src, Size);
	UpdateResourceBuffer->Unmap(0, nullptr);

	CD3DX12_RESOURCE_BARRIER ResourceBarrierCommonToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(Dest, FromState, D3D12_RESOURCE_STATE_COPY_DEST);
	CD3DX12_RESOURCE_BARRIER ResourceBarrierCopyDestToVertexBuffer = CD3DX12_RESOURCE_BARRIER::Transition(Dest, D3D12_RESOURCE_STATE_COPY_DEST, ToState);
	CommandList->ResourceBarrier(1, &ResourceBarrierCommonToCopyDest);
	CommandList->CopyBufferRegion(Dest, 0, UpdateResourceBuffer, SrcBufferOffset, Size);
	CommandList->ResourceBarrier(1, &ResourceBarrierCopyDestToVertexBuffer);

	return hr;
}

HRESULT DX12GALResourceUpdater::UpdateTexture(
	ID3D12GraphicsCommandList* CommandList,
	ID3D12Resource* Dest,
	const D3D12_SUBRESOURCE_DATA* pSrcData,
	int32 NumSubResource,
	int32 UploadBufferSize,
	D3D12_RESOURCE_STATES FromState,
	D3D12_RESOURCE_STATES ToState)
{
	SCOPE_PROFILE(UpdateTexture);
	HRESULT hr = S_OK;
	byte* pUploadBufferData = nullptr;
	CD3DX12_RANGE writeRange(0, 0); // We do not intend to read from this resource on the CPU.

	int32 RentBufferStartOffset = 0;
	ID3D12Resource* UpdateResourceBuffer = RentUpdateBuffer(RentBufferStartOffset, UploadBufferSize);

	CD3DX12_RESOURCE_BARRIER ResourceBarrierCommonToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(Dest, FromState, D3D12_RESOURCE_STATE_COPY_DEST);
	CD3DX12_RESOURCE_BARRIER ResourceBarrierCopyDestToVertexBuffer = CD3DX12_RESOURCE_BARRIER::Transition(Dest, D3D12_RESOURCE_STATE_COPY_DEST, ToState);

	CommandList->ResourceBarrier(1, &ResourceBarrierCommonToCopyDest);
	uint64 UpdateSize;
	{
		SCOPE_PROFILE(UpdateSubresource);
		UpdateSize = UpdateSubresources(CommandList, Dest, UpdateResourceBuffer, RentBufferStartOffset, 0, NumSubResource, pSrcData);
	}
	CommandList->ResourceBarrier(1, &ResourceBarrierCopyDestToVertexBuffer);


	if (UpdateSize <= 0)
	{
		SS_ASSERT(false);
		return S_FALSE;
	}

	return hr;
}

ID3D12Resource* DX12GALResourceUpdater::RentUpdateBuffer(int32& OutBufferStartOffset, int64 BufferSize)
{
	SCOPE_PROFILE(RentUpdateBuffer);
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)_AncestorOwnerRenderDevice)->GetD3DDevice();
	HRESULT hr;

	constexpr int64 BUFFERCOPY_ALIGN_SIZE = 512; // 몇몇 디바이스에선 512바이트 단위로 얼라인 돼있어야 텍스쳐 카피가 가능함
	int64 Buffer512Unit = BufferSize / BUFFERCOPY_ALIGN_SIZE + (BufferSize % BUFFERCOPY_ALIGN_SIZE == 0 ? 0 : 1);

	BufferSize = Buffer512Unit * BUFFERCOPY_ALIGN_SIZE;

	if (BufferSize > GAL_DEFAULT_RESOURCEUPDATE_TARGET_SIZE_MAX) // DefaultUploadPage를 잘라써서 쓰지 못하는 경우
	{
		int64 NeededPageCnt = BufferSize / LARGE_UPLOADBUFFER_SIZE_MIN
			+ (BufferSize % LARGE_UPLOADBUFFER_SIZE_MIN == 0 ? 0 : 1);
		// 만약 LARGE_UPLOADBUFFER_SIZE_MIN사이즈의 버퍼로만 업로드를 실행한다면
		// LARGE_UPLOADBUFFER_SIZE_MIN가 몇개가 필요한지 개수를 계산

		int64 PowCnt = 0;
		while (NeededPageCnt > (1 << PowCnt))
		{
			PowCnt++;
		}

		if (PowCnt >= LARGE_UPLOADBUFFER_TYPE_CNT)
		{
			SS_INTERRUPT(L"BufferSize is too big.");
			return nullptr;
		}

		SS::PooledList<ID3D12Resource*>& BufferList = _LargeUploadBuffers[PowCnt];
		int64 CurBufferIdx = _CurLargeUploadBufferIndices[PowCnt];

		if (BufferList.GetSize() <= CurBufferIdx)
		{
			SCOPE_PROFILE(CreateLargeUpdateBuffer);

			CD3DX12_HEAP_PROPERTIES UploadHeapTypeProp(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GAL_DEFAULT_RESOURCEUPDATE_TARGET_SIZE_MAX * (1 << PowCnt));

			ID3D12Resource* NewUpdateBuffer = nullptr;

			hr = D3DDevice->CreateCommittedResource(
				&UploadHeapTypeProp,
				D3D12_HEAP_FLAG_NONE,
				&ResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&NewUpdateBuffer));
			NewUpdateBuffer->SetName(L"ResourceUpdater");

			if (FAILED(hr))
			{
				SS_INTERRUPT();
				return nullptr;
			}

			BufferList.PushBack(NewUpdateBuffer);
		}

		_CurLargeUploadBufferIndices[PowCnt] = CurBufferIdx + 1;

		OutBufferStartOffset = 0;
		return BufferList[CurBufferIdx];
	}


	// DefaultUploadPage를 잘라서 쓰는 경우

	const int32 RemainingBufferSize = GAL_DEFAULT_RESOURCEUUPDATER_PAGESIZE - _CurBufferOffset;

	if (RemainingBufferSize < BufferSize)
	{
		_CurBufferOffset = 0;
		_CurDefaultBufferIdx++;
	}

	if (_DefaultUploadBuffers.GetSize() <= _CurDefaultBufferIdx) // 예약돼있는 페이지보다 더 많은 양이 필요한 경우 증량
	{
		CD3DX12_HEAP_PROPERTIES UploadHeapTypeProp(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GAL_DEFAULT_RESOURCEUUPDATER_PAGESIZE);

		ID3D12Resource* NewUpdateBuffer = nullptr;

		hr = D3DDevice->CreateCommittedResource(
			&UploadHeapTypeProp,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&NewUpdateBuffer));
		NewUpdateBuffer->SetName(L"ResourceUpdater");

		if (FAILED(hr))
		{
			SS_INTERRUPT();
			return nullptr;
		}

		_DefaultUploadBuffers.PushBack(NewUpdateBuffer);
	}

	OutBufferStartOffset = _CurBufferOffset;
	ID3D12Resource* OutUploadBuffer = _DefaultUploadBuffers[_CurDefaultBufferIdx];

	_CurBufferOffset += BufferSize;

	return OutUploadBuffer;
}
