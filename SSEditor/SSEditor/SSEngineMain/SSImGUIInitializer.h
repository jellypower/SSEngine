#pragma once
#include "pch.h"
#include "imgui.h"
#include "SSGAL/Public/SSGALInlineSettings.h"

#include "SSGAL/Public/ModuleEntry/ExtractD3DDeviceInfo.h"


class IRenderer;

struct ExampleDescriptorHeapAllocator
{
	ID3D12DescriptorHeap* Heap = nullptr;
	D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
	D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
	UINT                        HeapHandleIncrement;
	ImVector<int>               FreeIndices;

	void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
	{
		IM_ASSERT(Heap == nullptr && FreeIndices.empty());
		Heap = heap;
		D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
		HeapType = desc.Type;
		HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
		HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
		HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
		FreeIndices.reserve((int)desc.NumDescriptors);
		for (int n = desc.NumDescriptors; n > 0; n--)
			FreeIndices.push_back(n - 1);
	}
	void Destroy()
	{
		Heap = nullptr;
		FreeIndices.clear();
	}
	void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		IM_ASSERT(FreeIndices.Size > 0);
		int idx = FreeIndices.back();
		FreeIndices.pop_back();
		out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
		out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
	}
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
	{
		int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
		int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
		IM_ASSERT(cpu_idx == gpu_idx);
		FreeIndices.push_back(cpu_idx);
	}
};


extern class SSImGUIInitializer* g_ImGuiInitializer;
class SSImGUIInitializer
{
public:
	ExampleDescriptorHeapAllocator _ImGUIDescriptorHeapAllocator;

private:
	ID3D12Device* _D3DDeviceCache = nullptr;
	ID3D12CommandQueue* _D3DCommandQueueCache = nullptr;
	ID3D12DescriptorHeap* _ImguiDescriptorHeap = nullptr;
	int32 _NestedFrameCnt = 0;
	ID3D12CommandAllocator* _CommandAllocator[SWAP_CHAIN_FRAME_COUNT] = { nullptr, };
	ID3D12GraphicsCommandList* _CommandList[SWAP_CHAIN_FRAME_COUNT] = { nullptr, };
	int32 _CurSwapChainIdx = 0;


	FuncPtr_ExtractD3DDeviceInfo _fpExtractD3DDeviceInfo = nullptr;
	FuncPtr_QueryCurrentD3DGALDeviceContext _fpQueryCurrentD3DGALDeviceContext = nullptr;

	IRenderer* _Renderer = nullptr;

public:
	void StartupImGui(IRenderer* InRenderer);
	void CleanUpImGui();

	void OnBeginFrameImGui();
	void OnEndFrameImGui();
};

void Run_g_ImGuiInitializer__OnBeginFrameImGui();
void Run_g_ImGuiInitializer_OnEndFrameImGui();
