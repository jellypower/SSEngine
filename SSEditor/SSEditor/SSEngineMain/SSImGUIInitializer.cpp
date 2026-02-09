#include "pch.h"

#include "SSImGUIInitializer.h"

#include "ModuleEntryScriptRunner.h"

#include "SSGAL/Public/ModuleEntry/GALInstanceFactory.h"

#include "SSRenderer/Public/RenderBase/IRenderer.h"


SSImGUIInitializer* g_ImGuiInitializer = nullptr;

void SSImGUIInitializer::StartupImGui(IRenderer* InRenderer)
{
	_Renderer = InRenderer;

	// Extract D3D Devices
	_fpExtractD3DDeviceInfo = (FuncPtr_ExtractD3DDeviceInfo)GetProcAddress(g_hInstSSGAL, "ExtractD3DDeviceInfo");
	_fpExtractD3DDeviceInfo(
		_Renderer->GetRenderDevice(),
		&_D3DDeviceCache,
		&_D3DCommandQueueCache,
		&_NestedFrameCnt);

	_fpQueryCurrentD3DGALDeviceContext = 
		(FuncPtr_QueryCurrentD3DGALDeviceContext)GetProcAddress(g_hInstSSGAL, "QueryCurrentD3DGALDeviceContext");


	// Command List
	{
		for (int32 i = 0; i < _NestedFrameCnt; i++)
		{
			ID3D12CommandAllocator* NewCommandAllocator = nullptr;
			ID3D12GraphicsCommandList* NewCommandList = nullptr;
			if (FAILED(_D3DDeviceCache->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&NewCommandAllocator))))
			{
				SS_INTERRUPT();
			}

			if (FAILED(_D3DDeviceCache->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, NewCommandAllocator, nullptr, IID_PPV_ARGS(&NewCommandList))))
			{
				SS_INTERRUPT();
			}


			NewCommandList->Close();

			_CommandAllocator[i] = NewCommandAllocator;
			_CommandList[i] = NewCommandList;
		}
	}


	// Descriptor Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 64;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hr = _D3DDeviceCache->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_ImguiDescriptorHeap));
		if (FAILED(hr))
		{
			SS_INTERRUPT();
		}

		_ImGUIDescriptorHeapAllocator.Create(_D3DDeviceCache, _ImguiDescriptorHeap);
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());

	ImGui_ImplWin32_Init(g_hWnd);

	// Setup Platform/Renderer backends
	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = _D3DDeviceCache;
	init_info.CommandQueue = _D3DCommandQueueCache;
	init_info.NumFramesInFlight = SWAP_CHAIN_FRAME_COUNT;
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // Or your render target format.
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// The example_win32_directx12/main.cpp application include a simple free-list based allocator.
	init_info.SrvDescriptorHeap = _ImguiDescriptorHeap;
	init_info.SrvDescriptorAllocFn =
		[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
		{ return g_ImGuiInitializer->_ImGUIDescriptorHeapAllocator.Alloc(out_cpu_handle, out_gpu_handle); };
	init_info.SrvDescriptorFreeFn =
		[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
		{ return g_ImGuiInitializer->_ImGUIDescriptorHeapAllocator.Free(cpu_handle, gpu_handle); };


	ImGui_ImplDX12_Init(&init_info);
}

void SSImGUIInitializer::CleanUpImGui()
{
	_ImGUIDescriptorHeapAllocator.Destroy();
	_ImguiDescriptorHeap->Release();

	for (int i = 0; i < _NestedFrameCnt; i++)
	{
		_CommandList[i]->Release();
	}

	for (int i = 0; i < _NestedFrameCnt; i++)
	{
		_CommandAllocator[i]->Release();
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void SSImGUIInitializer::OnBeginFrameImGui()
{
	// (Your code process and dispatch Win32 messages)
// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void SSImGUIInitializer::OnEndFrameImGui()
{
	// Rendering
// (Your code clears your framebuffer, renders your other stuff etc.)

	ID3D12CommandAllocator* CurCommandAllocator = _CommandAllocator[_CurSwapChainIdx];
	ID3D12GraphicsCommandList* CurCommandList = _CommandList[_CurSwapChainIdx];
	D3D12_RESOURCE_BARRIER barrier = {};


	ImGui::Render(); // IMGUI


	{
		CurCommandAllocator->Reset();
		CurCommandList->Reset(CurCommandAllocator, nullptr);

		ID3D12Resource* SwapChainBufferToDraw = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE SwapChainHandleToDraw;

		_fpQueryCurrentD3DGALDeviceContext(_Renderer->GetRenderDevice(), &SwapChainBufferToDraw, &SwapChainHandleToDraw);

		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = SwapChainBufferToDraw;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		CurCommandList->ResourceBarrier(1, &barrier);
		CurCommandList->OMSetRenderTargets(1, &SwapChainHandleToDraw, FALSE, nullptr);
		CurCommandList->SetDescriptorHeaps(1, &_ImguiDescriptorHeap);
	}


	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _CommandList[_CurSwapChainIdx]);
	// (Your code calls ExecuteCommandLists, swapchain's Present(), etc.)

	{
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		CurCommandList->ResourceBarrier(1, &barrier);
		CurCommandList->Close();
		_D3DCommandQueueCache->ExecuteCommandLists(1, (ID3D12CommandList* const*)&CurCommandList);
	}

	_CurSwapChainIdx++;
	_CurSwapChainIdx = _CurSwapChainIdx % SWAP_CHAIN_FRAME_COUNT;
}

void Run_g_ImGuiInitializer__OnBeginFrameImGui()
{
	g_ImGuiInitializer->OnBeginFrameImGui();
}

void Run_g_ImGuiInitializer_OnEndFrameImGui()
{
	g_ImGuiInitializer->OnEndFrameImGui();
}