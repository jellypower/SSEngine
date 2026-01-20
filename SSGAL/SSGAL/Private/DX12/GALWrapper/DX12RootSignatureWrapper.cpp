#include "pch.h"


#include "DX12RootSignatureWrapper.h"


#include "SSGAL/Private/DX12/GALRenderDevice/DX12GALRenderDevice.h"
#include "SSGAL/Private/PCommon/GALWrapper/RootSignaturePool.h"

DX12RootSignatureWrapper::DX12RootSignatureWrapper(ERootSignatureType RootSignatureType, RootSignaturePool* InOwnerRootSignaturePool)
	: RootSignatureWrapper(RootSignatureType, InOwnerRootSignaturePool)
{
	DX12GALRenderDevice* OwnerRenderDevice = (DX12GALRenderDevice*)_OwnerRenderSignaturePool->GetOwnerRenderDeivce();
	ID3D12Device5* D3DDevice = OwnerRenderDevice->GetD3DDevice();

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

			static CD3DX12_DESCRIPTOR_RANGE MaterialTextures[5] = {};
			MaterialTextures[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0 
			MaterialTextures[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);	// t1
			MaterialTextures[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);	// t2 
			MaterialTextures[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);	// t3 
			MaterialTextures[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);	// t4

			static CD3DX12_DESCRIPTOR_RANGE WorldRenderEnv[1] = {};
			WorldRenderEnv[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);	// t5

			static CD3DX12_DESCRIPTOR_RANGE SkinningJoint[2] = {};
			SkinningJoint[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);	// t10
			SkinningJoint[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);	// t11

			static CD3DX12_ROOT_PARAMETER rootParameters[7] = {};

			rootParameters[0].InitAsConstantBufferView(0); // b0
			rootParameters[1].InitAsConstantBufferView(1); // b1
			rootParameters[2].InitAsConstantBufferView(2); // b2
			rootParameters[3].InitAsDescriptorTable(_countof(MaterialTextures), MaterialTextures, D3D12_SHADER_VISIBILITY_ALL); // textures

			rootParameters[4].InitAsConstantBufferView(3); // b3
			rootParameters[5].InitAsDescriptorTable(_countof(WorldRenderEnv), WorldRenderEnv, D3D12_SHADER_VISIBILITY_ALL);

			rootParameters[6].InitAsDescriptorTable(_countof(SkinningJoint), SkinningJoint, D3D12_SHADER_VISIBILITY_ALL);
			

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			static D3D12_STATIC_SAMPLER_DESC sampler = GetSamplerDescOfType(ESSSamplerDescType::DefaultSampler, 0);

			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);
		}

		return rootSignatureDesc;
	}
	case ERootSignatureType::Shadow:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false)
		{
			Initialized = true;

			static CD3DX12_DESCRIPTOR_RANGE SkinningJoint[2] = {};
			SkinningJoint[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);	// t10
			SkinningJoint[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);	// t11

			static CD3DX12_ROOT_PARAMETER rootParameters[3] = {};
			rootParameters[0].InitAsConstantBufferView(0); // b0 ModelBuffer
			rootParameters[1].InitAsConstantBufferView(1); // b1 RenderEnvParam
			rootParameters[2].InitAsDescriptorTable(_countof(SkinningJoint), SkinningJoint, D3D12_SHADER_VISIBILITY_VERTEX); // Skinning


			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			static D3D12_STATIC_SAMPLER_DESC sampler = GetSamplerDescOfType(ESSSamplerDescType::DefaultSampler, 0);

			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

			return rootSignatureDesc;
		}
	}
	case ERootSignatureType::DeferredShading:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false)
		{
			Initialized = true;

			
			static CD3DX12_DESCRIPTOR_RANGE GBufferTextures[5] = {};
			GBufferTextures[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // float3 Normal
			GBufferTextures[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // float3 Albedo
			GBufferTextures[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // float3 WorldPos
			GBufferTextures[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); // float2 MetallicRoughness
			GBufferTextures[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4); // float3 Emissive

			static CD3DX12_DESCRIPTOR_RANGE ShadowMapTextures[1] = {};
			ShadowMapTextures[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); // float ShadowMap

			static CD3DX12_ROOT_PARAMETER rootParameters[4] = {};
			rootParameters[0].InitAsConstantBufferView(0); // RenderLight
			rootParameters[1].InitAsConstantBufferView(1); // RenderEnv
			rootParameters[2].InitAsDescriptorTable(_countof(GBufferTextures), GBufferTextures, D3D12_SHADER_VISIBILITY_ALL); // G-Buffer
			rootParameters[3].InitAsDescriptorTable(_countof(ShadowMapTextures), ShadowMapTextures, D3D12_SHADER_VISIBILITY_ALL); // ShadowMap

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

			static D3D12_STATIC_SAMPLER_DESC samplers[] =
			{
				GetSamplerDescOfType(ESSSamplerDescType::DefaultSampler, 0),
				GetSamplerDescOfType(ESSSamplerDescType::ShadowCmpSmapler, 1)
			};

			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, _countof(samplers), samplers, rootSignatureFlags);

		}return rootSignatureDesc;
	}
	case ERootSignatureType::SkyMap:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false) {
			Initialized = true;

			static CD3DX12_DESCRIPTOR_RANGE SkyMapTexture[1] = {}; 
			SkyMapTexture[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0


			static CD3DX12_ROOT_PARAMETER rootParameters[3] = {};

			rootParameters[0].InitAsConstantBufferView(0); // b0
			rootParameters[1].InitAsConstantBufferView(1); // b1
			rootParameters[2].InitAsDescriptorTable(_countof(SkyMapTexture), SkyMapTexture); // textures

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			static D3D12_STATIC_SAMPLER_DESC sampler = GetSamplerDescOfType(ESSSamplerDescType::DefaultCubeMapSampler, 0);

			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);
		}return rootSignatureDesc;
	}
	case ERootSignatureType::DebugWire:
	{
		static CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		static bool Initialized = false;

		if (Initialized == false) {
			Initialized = true;



			static CD3DX12_ROOT_PARAMETER rootParameters[3] = {};

			rootParameters[0].InitAsConstantBufferView(0); // b0
			rootParameters[1].InitAsConstantBufferView(1); // b1
			rootParameters[2].InitAsConstantBufferView(2); // b2

			static D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


			rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);
		}return rootSignatureDesc;
	}
	}

	SS_INTERRUPT();
	return CD3DX12_ROOT_SIGNATURE_DESC();
}

D3D12_STATIC_SAMPLER_DESC DX12RootSignatureWrapper::GetSamplerDescOfType(ESSSamplerDescType InSamplerType, int32 registerIDX)
{
	switch (InSamplerType)
	{
	case ESSSamplerDescType::DefaultSampler:
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
	case ESSSamplerDescType::ShadowCmpSmapler:
	{
		D3D12_STATIC_SAMPLER_DESC newSampler = {};
		newSampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		newSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		newSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		newSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		newSampler.MipLODBias = 0.0f;
		newSampler.MaxAnisotropy = 16;
		newSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		newSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		newSampler.MinLOD = -FLT_MAX;
		newSampler.MaxLOD = D3D12_FLOAT32_MAX;
		newSampler.ShaderRegister = registerIDX;
		newSampler.RegisterSpace = 0;
		newSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		return newSampler;
	}
	case ESSSamplerDescType::DefaultCubeMapSampler:
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
