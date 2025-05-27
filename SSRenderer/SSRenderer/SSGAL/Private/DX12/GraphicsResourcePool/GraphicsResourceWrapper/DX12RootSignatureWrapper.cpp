#include "DX12RootSignatureWrapper.h"


#include "SSGAL/Private/GALInstanceGlobalVariablePrivate.h"
#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"

DX12RootSignatureWrapper::DX12RootSignatureWrapper(ERootSignatureType RootSignatureType)
	: RootSignatureWrapper(RootSignatureType)
{
	ID3D12Device5* D3DDevice = ((DX12GALRenderDevice*)SSGALModule::Private::g_GALRenderDevice)->GetD3DDevice();

	ID3DBlob* Signature = nullptr;
	ID3DBlob* Error = nullptr;

	const CD3DX12_ROOT_SIGNATURE_DESC& rootSignatureDesc = GetRootSignatureDescOfType(RootSignatureType);


	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error)))
	{
		char* errStr = (char*)Error->GetBufferPointer();
		DEBUG_BREAK();
		return;
	}

	if (FAILED(D3DDevice->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&_D3DRootSignature))))
	{
		DEBUG_BREAK();
		return;
	}

	if (Signature)
	{
		Signature->Release();
		Signature = nullptr;
	}

	if (Error)
	{
		Error->Release();
		Error = nullptr;
	}

	_D3DRootSignature->SetName(ToUtf16Str(RootSignatureType));
}

DX12RootSignatureWrapper::~DX12RootSignatureWrapper()
{
	if (_D3DRootSignature != nullptr)
	{
		_D3DRootSignature->Release();
		_D3DRootSignature = nullptr;
	}
}

bool DX12RootSignatureWrapper::IsValid() const
{
	return _D3DRootSignature != nullptr;
}

const CD3DX12_ROOT_SIGNATURE_DESC& DX12RootSignatureWrapper::GetRootSignatureDescOfType(
	ERootSignatureType InRootSignatureType)
{
	switch (InRootSignatureType)
	{
	case ERootSignatureType::NONE:
		DEBUG_BREAK();
		return CD3DX12_ROOT_SIGNATURE_DESC();

	case ERootSignatureType::SS_TEMP_ROOTSIGNATURE:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false)
		{
			Initialized = true;

			static CD3DX12_ROOT_PARAMETER rootParams[3] = {};
			rootParams[0].InitAsConstantBufferView(0); // b0
			rootParams[1].InitAsConstantBufferView(1); // b1
			rootParams[2].InitAsConstantBufferView(2); // b2

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			rootSignatureDesc.Init(_countof(rootParams), rootParams, 0 , nullptr, rootSignatureFlags);
		}

		return rootSignatureDesc;
	}
	case ERootSignatureType::SS_DEFAULT_PBR:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false) {
			Initialized = true;

			static CD3DX12_DESCRIPTOR_RANGE texDescTable[5] = {};
			texDescTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0 
			texDescTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);	// t1
			texDescTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);	// t2 
			texDescTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);	// t3 
			texDescTable[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);	// t4 

			static CD3DX12_ROOT_PARAMETER rootParameters[4] = {};
			rootParameters[0].InitAsConstantBufferView(0); // b0
			rootParameters[1].InitAsConstantBufferView(1); // b1
			rootParameters[2].InitAsConstantBufferView(2); // b2
			rootParameters[3].InitAsDescriptorTable(_countof(texDescTable), texDescTable, D3D12_SHADER_VISIBILITY_ALL); // textures

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			static D3D12_STATIC_SAMPLER_DESC sampler = GetSamplerDescOfType(EGRD3D12SamplerDescType::DefaultSampler, 0);

			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);
		}

		return rootSignatureDesc;
	}

	}
}

D3D12_STATIC_SAMPLER_DESC DX12RootSignatureWrapper::GetSamplerDescOfType(EGRD3D12SamplerDescType InSamplerType, int32 registerIDX)
{
	switch (InSamplerType)
	{
	case EGRD3D12SamplerDescType::DefaultSampler:
	{
		D3D12_STATIC_SAMPLER_DESC newSampler = {};
		newSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		newSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		newSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		newSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		newSampler.MipLODBias = 0.0f;
		newSampler.MaxAnisotropy = 16;
		newSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		newSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		newSampler.MinLOD = -FLT_MAX;
		newSampler.MaxLOD = D3D12_FLOAT32_MAX;
		newSampler.ShaderRegister = registerIDX;
		newSampler.RegisterSpace = 0;
		newSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		return newSampler;
	}

	}

	SS_INTERRUPT();
	return D3D12_STATIC_SAMPLER_DESC();
}
