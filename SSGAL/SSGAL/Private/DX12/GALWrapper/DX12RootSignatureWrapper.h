#pragma once
#include <d3dx12.h>

#include "SSGAL/Private/PCommon/GALWrapper/RootSignatureWrapper.h"

enum class EGRD3D12SamplerDescType
{
	None = -1,

	DefaultSampler
};


class DX12RootSignatureWrapper : public RootSignatureWrapper
{
private:
	ID3D12RootSignature* _D3DRootSignature = nullptr;

public:
	DX12RootSignatureWrapper(ERootSignatureType RootSignatureType, RootSignaturePool* InOwnerRootSignaturePool);
	virtual ~DX12RootSignatureWrapper();

	ID3D12RootSignature* GetRootSignatureInstantce() const { return _D3DRootSignature; }
	virtual bool IsValid() const override;

private:
	static const CD3DX12_ROOT_SIGNATURE_DESC& GetRootSignatureDescOfType(ERootSignatureType InRootSignatureType);
	static D3D12_STATIC_SAMPLER_DESC GetSamplerDescOfType(EGRD3D12SamplerDescType InSamplerType, int32 registerIDX);
};
