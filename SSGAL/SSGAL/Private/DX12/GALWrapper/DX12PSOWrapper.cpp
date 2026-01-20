#include "pch.h"


#include "DX12PSOWrapper.h"

#include "DX12GALShaderWrapper.h"
#include "DX12RootSignaturePool.h"
#include "DX12RootSignatureWrapper.h"
#include "Private/DX12/Utils/SSDX12Utils.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/PCommon/GALWrapper/PSOPool.h"
#include "SSGAL/Public/GALWrapper/GALShaderPool.h"




const D3D12_INPUT_ELEMENT_DESC* DX12PSOWrapper::GetInputElementDesc(EInputLayoutType InputElementType, uint32& outElementCnt)
{
	switch (InputElementType)
	{
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, 48,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,	0, 56,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,	0, 64,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 1, DXGI_FORMAT_R32G32B32A32_UINT,	0, 80,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 96,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 112,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		outElementCnt = _countof(inputElementDesc);
		return inputElementDesc;
	}
	case EInputLayoutType::SS_INPUTLAYOUT_NULL:
	{
		outElementCnt = 0;
		return nullptr;
	}
	default:
	{
		DEBUG_BREAK();
		outElementCnt = 0;
		return nullptr;
	}
	}
}

DX12PSOWrapper::DX12PSOWrapper(const PipelineDesc& InPipelineDesc, PSOPool* InOwnerPSOPool)
	: PSOWrapper(InPipelineDesc, InOwnerPSOPool)
{
	DX12GALRenderDevice* GALDevice = (DX12GALRenderDevice*)InOwnerPSOPool->GetOwnerDevice();
	ID3D12Device5* D3DDevice = GALDevice->GetD3DDevice();
	GALShaderPool* ShaderPool = GALDevice->GetShaderPool();
	DX12RootSignaturePool* RootSignaturePool = (DX12RootSignaturePool*)GALDevice->GetRootSignaturePool();


	if (InPipelineDesc.CSName.IsEmpty()) // Compute Shader가 아닌 경우
	{
		const DX12RootSignatureWrapper* RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignaturePool->GetRootSignature(InPipelineDesc.RootSignatureType);
		if (RootSignatureWrapper->IsValid() == false)
		{
			DEBUG_BREAK();
			return;
		}
		ID3D12RootSignature* RootSignature = RootSignatureWrapper->GetRootSignatureInstantce();


		const DX12GALShaderWrapper* VS = (DX12GALShaderWrapper*)ShaderPool->FindShader(InPipelineDesc.VSName);
		if (VS == nullptr)
		{
			DEBUG_BREAK();
			return;
		}
		ID3DBlob* VSBlob = VS->GetCompiledShader();

		ID3DBlob* PSBlob = nullptr;
		if (InPipelineDesc.PSName.IsEmpty() == false)
		{
			const DX12GALShaderWrapper* PS = (DX12GALShaderWrapper*)ShaderPool->FindShader(InPipelineDesc.PSName);
			if (PS == nullptr)
			{
				DEBUG_BREAK();
				return;
			}

			PSBlob = PS->GetCompiledShader();
		}


		uint32 inputElementCnt = 0;
		const D3D12_INPUT_ELEMENT_DESC* inputElementDesc = GetInputElementDesc(InPipelineDesc.LayoutType, inputElementCnt);

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};


		psoDesc.InputLayout = { inputElementDesc, inputElementCnt };
		psoDesc.pRootSignature = RootSignature;
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize());
		if (PSBlob != nullptr) psoDesc.PS = CD3DX12_SHADER_BYTECODE(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.RasterizerState.CullMode = InPipelineDesc.bUseTwoSideRender ? D3D12_CULL_MODE_NONE : D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.FillMode = InPipelineDesc.bUseWireFrame ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = InPipelineDesc.NumRenderTarget;
		for (int32 i = 0; i < InPipelineDesc.NumRenderTarget; i++)
		{
			psoDesc.RTVFormats[i] = SS::DX12Util::ConvertColorFormat(InPipelineDesc.RTColorFormats[i]);
		}
		psoDesc.DSVFormat = SS::DX12Util::ConvertColorFormat(InPipelineDesc.DSColorFormat);
		psoDesc.SampleDesc.Count = 1;


		HRESULT result = D3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_PipelineState));
		if (FAILED(result))
		{
			_PipelineState = nullptr;
			SS_INTERRUPT("CreatePipeline Failed.");
			return;
		}
	}
	else // Compute Shader인 경우
	{
		SS_INTERRUPT(); // TODO: 아직 미구현된 기능입니다.

		const DX12RootSignatureWrapper* RootSignatureWrapper = (const DX12RootSignatureWrapper*)RootSignaturePool->GetRootSignature(InPipelineDesc.RootSignatureType);
		if (RootSignatureWrapper->IsValid() == false)
		{
			DEBUG_BREAK();
			return;
		}
		ID3D12RootSignature* RootSignature = RootSignatureWrapper->GetRootSignatureInstantce();


		const DX12GALShaderWrapper* CS = (DX12GALShaderWrapper*)ShaderPool->FindShader(InPipelineDesc.CSName);
		if (CS == nullptr)
		{
			DEBUG_BREAK();
			return;
		}
		ID3DBlob* CSBlob = CS->GetCompiledShader();

		const D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {
			.pRootSignature = RootSignature,
			.CS = CD3DX12_SHADER_BYTECODE(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize()),
			.NodeMask = 0,
			.CachedPSO = {.pCachedBlob = NULL, .CachedBlobSizeInBytes = 0 },
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};

		D3DDevice->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&_PipelineState));
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
