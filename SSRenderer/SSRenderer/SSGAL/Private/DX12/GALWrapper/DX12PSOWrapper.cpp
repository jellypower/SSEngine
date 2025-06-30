#include "DX12PSOWrapper.h"

#include "DX12RootSignaturePool.h"
#include "DX12RootSignatureWrapper.h"
#include "SSGAL/Private/GALInstanceGlobalVariablePrivate.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

#include "SSGAL/Private/DX12/GPURenderAsset/DX12GPUShaderAssetInstance.h"
#include "SSRenderer/Public/RenderAsset/ShaderAssetManager.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/ShaderAsset.h"
#include "SSRenderer/Public/RenderBase/SSRenderer.h"


const D3D12_INPUT_ELEMENT_DESC* DX12PSOWrapper::GetInputElementDesc(EInputLayoutType InputElementType, uint32& outElementCnt)
{
	switch (InputElementType)
	{
	case EInputLayoutType::NONE:
	{
		DEBUG_BREAK();
		outElementCnt = 0;
		return nullptr;
	}
	case EInputLayoutType::SS_DEFAULT_VS_RIGID_VERTEX_LAYOUT:
	{
		static D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, 48,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,	0, 56,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		outElementCnt = _countof(inputElementDesc);
		return inputElementDesc;
	}
	case EInputLayoutType::SS_DEFAULT_VS_SKIN_VERTEX_LAYOUT:
	{
		static D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		};

		DEBUG_BREAK(); // TODO: Skinning 구현하기 2024/12/31

		outElementCnt = _countof(inputElementDesc);
		return inputElementDesc;
	}
	}
}

DX12PSOWrapper::DX12PSOWrapper(const PipelineDesc& pipelineDesc)
	: PSOWrapper(pipelineDesc)
{
	DX12RootSignaturePool* RootSignaturePool = (DX12RootSignaturePool*)(SSGALModule::Private::g_GALRenderDevice)->GetRootSignaturePool();
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)SSGALModule::Private::g_GALRenderDevice)->GetD3DDevice();
	ShaderAssetManager* lShaderAssetManager = SSGALModule::Private::g_Renderer->GetShaderAssetManager();


	const DX12RootSignatureWrapper* RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignaturePool->GetRootSignature(pipelineDesc.RootSignatureType);
	if (RootSignatureWrapper->IsValid() == false)
	{
		DEBUG_BREAK();
		return;
	}
	ID3D12RootSignature* RootSignature = RootSignatureWrapper->GetRootSignatureInstantce();


	const ShaderAsset* VS = lShaderAssetManager->FindShaderAsset(pipelineDesc.VSName);
	if (VS == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	const ShaderAsset* PS = lShaderAssetManager->FindShaderAsset(pipelineDesc.PSName);
	if (PS == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	DX12GPUShaderAssetInstance* VSShaderGPUInstance = (DX12GPUShaderAssetInstance*)VS->GetGPUInstance();
	if (VSShaderGPUInstance == nullptr)
	{
		DEBUG_BREAK();
		return;
	}


	DX12GPUShaderAssetInstance* PSShaderGPUInstance = (DX12GPUShaderAssetInstance*)PS->GetGPUInstance();
	if (PSShaderGPUInstance == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	ID3DBlob* VSBlob = VSShaderGPUInstance->GetCompiledShader();
	ID3DBlob* PSBlob = PSShaderGPUInstance->GetCompiledShader();


	uint32 inputElementCnt = 0;
	const D3D12_INPUT_ELEMENT_DESC* inputElementDesc = GetInputElementDesc(pipelineDesc.LayoutType, inputElementCnt);
	if (inputElementDesc == nullptr)
	{
		DEBUG_BREAK();
		return;
	}

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};


	psoDesc.InputLayout = { inputElementDesc, inputElementCnt };
	psoDesc.pRootSignature = RootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;


	HRESULT result = D3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_PipelineState));
	if (FAILED(result))
	{
		_PipelineState = nullptr;
		SS_CLASS_ERR_LOG("CreatePipeline Failed.");
		return;
	}
}

DX12PSOWrapper::~DX12PSOWrapper()
{
	if (_PipelineState != nullptr)
	{
		_PipelineState->Release();
		_PipelineState = nullptr;
	}
}

bool DX12PSOWrapper::IsValid() const
{
	return _PipelineState != nullptr;
}
