#include "pch.h"

#include "Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "Private/PCommon/GALRenderDevice/PCommonGALRenderDevice.h"
#include "SSEngineDefault/Public/SSCommonUtil/SSCustomMemAllocator.h"
#include "SSEngineDefault/Public/RawProfiler/ProfilerUtils.h"


#include "Private/PCommon/TestCodes/GALTestCodes.h"

#include "Public/GALConstantBufferAccessorTypes/CBAModelBuffer.h"


void TestCustomChunkAllocator(PCommonGALRenderDevice* GALDevice)
{
	SSCustomMemChunkAllocator* CustomCBAllocator = GALDevice->GetConstantBufferResourceAllocator();
	SSCustomMemChunkAllocator* CustomDescTbleAllocator = GALDevice->GetDescriptorTableAllocator();
	ID3D12Device5* DX12Device = ((DX12GALRenderDevice*)GALDevice)->GetD3DDevice();

	constexpr int32 EACH_CHUNK_SIZE = 1024;
	constexpr int32 TEST_CHUNK_CNT = 2048;

	SS::PooledList<int32> RandReleaseIdx(TEST_CHUNK_CNT);

	for (int32 i = 0; i < TEST_CHUNK_CNT; i++)
	{
		RandReleaseIdx.PushBack(i);
	}

	srand(time(NULL));
	srand(time(NULL));
	for (int32 i = 0; i < TEST_CHUNK_CNT; i++)
	{
		int32 RandIdx1 = rand() % TEST_CHUNK_CNT;
		int32 RandIdx2 = rand() % TEST_CHUNK_CNT;

		int32 Tmp = RandReleaseIdx[RandIdx2];
		RandReleaseIdx[RandIdx2] = RandReleaseIdx[RandIdx1];
		RandReleaseIdx[RandIdx1] = Tmp;
	}

	uint64 CustomAllocatorTickCnt = 0;
	{
		AllocatedChunkHeader* DescSets = (AllocatedChunkHeader*)DBG_MALLOC(sizeof(AllocatedChunkHeader) * TEST_CHUNK_CNT);

		uint64 StartTickCnt = GetPerofrmanceCounter();
		for (int32 i = 0; i < TEST_CHUNK_CNT; i++)
		{
			DescSets[i] = CustomCBAllocator->AllocChunk(EACH_CHUNK_SIZE);
		}

		for (int32 i=0;i<TEST_CHUNK_CNT;i++)
		{
			int32 ReleaseIdx = RandReleaseIdx[i];
			CustomCBAllocator->ReleaseChunk(DescSets[ReleaseIdx]);
		}
		uint64 EndTickCnt = GetPerofrmanceCounter();

		CustomAllocatorTickCnt = EndTickCnt - StartTickCnt;

		free(DescSets);
	}

	SS_ASSERT(CustomCBAllocator->IsAnyChunkInUse() == false);

	uint64 CommittedResourecTickCnt = 0;
	{
		ID3D12Resource** Resources = (ID3D12Resource**)DBG_MALLOC(sizeof(ID3D12Resource*) * TEST_CHUNK_CNT);

		ID3D12Resource* newResource = nullptr;
		CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(EACH_CHUNK_SIZE);

		uint64 StartTickCnt = GetPerofrmanceCounter();
		for (int32 i = 0; i < TEST_CHUNK_CNT; i++)
		{
			HRESULT hr = DX12Device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&newResource));
			if (FAILED(hr))
			{
				DEBUG_BREAK();
				return;
			}

			newResource->SetName(L"TestCustomChunkAllocator");
			Resources[i] = newResource;
		}

		for (int32 i = 0; i < TEST_CHUNK_CNT; i++)
		{
			int32 ReleaseIdx = RandReleaseIdx[i];

			Resources[ReleaseIdx]->Release();
		}
		uint64 EndTickCnt = GetPerofrmanceCounter();

		CommittedResourecTickCnt = EndTickCnt - StartTickCnt;

		free(Resources);
	}

	double TimeScale = (double)CommittedResourecTickCnt / (double)CustomAllocatorTickCnt;
	int a = 0;
}
